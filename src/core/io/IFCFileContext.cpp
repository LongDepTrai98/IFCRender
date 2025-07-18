#include "IFCFileContext.hpp"
#include "IFCGeometryCache.hpp"
#include "threepp/threepp.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include "core/io/factory/GeometryCacheOffsetFactory.hpp"
#include "raycast/CustomRayCaster.hpp"
#include "spdlog/spdlog.h"
#include "ui/ElementTreeCtrl.hpp"
#include "core/utils/ThreeHelper.hpp"
#include "input/input.hpp"
#include "resource.hpp"
#include "threepp/materials/RawShaderMaterial.hpp"
#include <format>
#include <iostream>
#include <set>
namespace dragon
{
	IFCFileContext::IFCFileContext()
	{
		m_Model = std::make_unique<IFCModelCache>();
		/*INIT MATERIAL HOVER*/
		initLayerOverLay();
	}
	IFCFileContext::~IFCFileContext()
	{
		m_Model->clear();
		m_Toggle_Component_Callback = nullptr;
		m_Toggle_Components_Callback = nullptr;
		OnRedrawCallback = nullptr;
	}
	std::string IFCFileContext::getFileType()
	{
		return std::string("IFC");
	}
	IFCModelCache* IFCFileContext::getModelCache()
	{
		return m_Model.get();
	}
	void IFCFileContext::handleRaycast(CustomRayCaster& RayCaster, MouseState& mouse_state)
	{
		if (!m_bIsEnableHover) return;
		if (mouse_state.isLButtonDown)
		{
			m_Current_ExpressID = std::nullopt;
			return;
		}
		if (!m_Model->m_Object_Model)
		{
			m_Current_ExpressID = std::nullopt;
			return;
		}
		CustomRayCaster::Result result;
		bool hit = RayCaster.intersectObjects(result);
		if (hit)
		{
			const int& prim_id = result.prim_id;
			const int& index_face_a = 3 * prim_id;
			const int& index_face_b = 3 * prim_id + 1;
			const int& index_face_c = 3 * prim_id + 2;
			auto obj_geometry = m_Model->m_Object_Model->geometry();
			const int& a = m_Model->m_Object_Indices[index_face_a];
			const int& expressID = m_Model->m_Object_ExpressID[a];
			if (m_Current_ExpressID != expressID)
			{
				/*UPDATE EXPRESSID*/
				m_Current_ExpressID = expressID;
			}
			else if (!m_Current_ExpressID)
			{
				m_Current_ExpressID = expressID;
			}

			if (result.P != m_Coord_HitPoint)
			{
				m_Coord_HitPoint = result.P;
			}
		}
		else
		{
			m_Current_ExpressID = std::nullopt;
			m_Coord_HitPoint = std::nullopt;
		}
	}
	void IFCFileContext::initLayerOverLay()
	{
		/*HOVER LAYER OVERLAY*/
		if (!m_Material_Hover)
		{
			m_Material_Hover = threepp::MeshPhongMaterial::create();
			m_Material_Hover->as<threepp::MeshPhongMaterial>()->color = threepp::Color::darkgreen;
			m_Material_Hover->transparent = true;
			m_Material_Hover->depthTest = false;
			m_Material_Hover->opacity = 0.5f;
		}

		if (!m_Material_Hit_Point)
		{
			m_Material_Hit_Point = threepp::MeshBasicMaterial::create();
			m_Material_Hit_Point->color = threepp::Color::red;
		}

		if (!m_Custom_material)
		{
			m_Custom_material = threepp::RawShaderMaterial::create();
			const std::string vertexSource = R"(
			#version 330 core
				#define attribute in
				#define varying out
				uniform mat4 modelViewMatrix; // optional
				uniform mat4 projectionMatrix; // optional
				attribute vec3 position;
				attribute vec3 normal;
				uniform float outlineThickness;
				void main()
				{
					vec3 newPostion = position + normal * outlineThickness;
					gl_Position = projectionMatrix * modelViewMatrix * vec4( newPostion, 1.0 );
				}
			)";
			const std::string fragmentSource = R"(
				void main() {
					gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
				}
			)";

			m_Custom_material->vertexShader = vertexSource;
			m_Custom_material->fragmentShader = fragmentSource;
			m_Custom_material->side = threepp::Side::Back; 
			m_Custom_material->uniforms["outlineThickness"].setValue(0.02f);
			m_Custom_material->depthWrite = false; 
			m_Custom_material->polygonOffset = true; 
			m_Custom_material->polygonOffsetFactor = -4;
			m_Custom_material->polygonOffsetUnits = -4;
		}
		if (!m_Basic_Material)
		{
			m_Basic_Material = threepp::MeshBasicMaterial::create(); 
			m_Basic_Material->color = threepp::Color::lightgreen; 
			m_Basic_Material->side = threepp::Side::Back; 
		}
	}
	void IFCFileContext::handleHoverResult()
	{
		if (!m_bIsEnableHover) return;
		if (m_bIsHoverMode)
		{
			drawHoverLayer();
			if (OnRedrawCallback)
				OnRedrawCallback();
		}
		if (m_bIsSelectPivotMode)
		{
			updateCoordHitPoint();
			if (OnRedrawCallback)
				OnRedrawCallback();
		}
	}

	void IFCFileContext::rebuildVisibleIndices()
	{
		m_Hidden_Express_IDs.clear();
		std::map <int, std::vector<std::pair<int, int>>> view_geometries_with_materials{};
		size_t total_indices = 0;
		for (auto& [expressID, element] : m_Model->m_Geometry_Offset)
		{
			/*SHOW ELEMENT*/
			if (element.state == 1)
			{
				for (auto& offset : element.offsets)
				{
					int begin_offset = offset.begin_indices_offset;
					int end_offset = offset.end_indices_offset;
					total_indices += (end_offset - begin_offset) + 1;
					const int& index_material = offset.material_index;
					std::pair<int, int> pair = { begin_offset,end_offset };
					view_geometries_with_materials[index_material].emplace_back(pair);
				}
			}
			else
				m_Hidden_Express_IDs.insert({ expressID });
		}
		//update indices
		std::shared_ptr<threepp::BufferGeometry> sub_geometry = ThreeHelper::BuildSubGeometry(total_indices,
			m_Model->m_Object_Materials,
			view_geometries_with_materials,
			m_Model->m_Object_Indices,
			m_Model->m_Object_Vertices,
			m_Model->m_Object_Normals);
		m_Model->m_Object_Model->as<threepp::Mesh>()->setGeometry(sub_geometry);
		m_Current_ExpressID = std::nullopt;
	}

	void IFCFileContext::drawHoverLayer()
	{
		if (!m_Current_ExpressID)
		{
			m_Object_OverLay_Hover->visible = false;
			m_Old_ExpressID = std::nullopt;
		}
		if (m_Current_ExpressID && m_Current_ExpressID != m_Old_ExpressID) {
			spdlog::info("Draw overlay layer : {}", m_Current_ExpressID.value());
			/*CREATE HOVER OBJECT OVERLAY*/
			if (!m_Object_OverLay_Hover->visible)
			{
				m_Object_OverLay_Hover->visible = true;
			}
			/*GET OFFSET HOVER GEOMETRY*/
			auto it = m_Model->m_Geometry_Offset.find(m_Current_ExpressID.value());
			if (it != m_Model->m_Geometry_Offset.end())
			{
				const IFCModelCache::element& e = it->second;
				std::shared_ptr<threepp::BufferGeometry> geo_hover = ThreeHelper::BuildSubGeometryWithOffset(e,
					m_Model->m_Object_Vertices,
					m_Model->m_Object_Normals,
					m_Model->m_Object_Indices
				);
				threepp::Box3 box;
				box.setFromArray(geo_hover->getAttribute<float>("position")->array());
				auto center = box.getCenter();
				m_Object_OverLay_Hover->position.set(center.x, center.y, center.z);


				//threepp::Matrix4 T1, T2, S;
				//T1.makeTranslation(threepp::Vector3(-center.x,-center.y,-center.z));
				//S.makeScale(2.0f, 2.0f, 2.0f);
				//T2.makeTranslation(threepp::Vector3(center.x, center.y, center.z));

				//// finalMatrix = T1 * S * T2 * world
				//threepp::Matrix4 finalMatrix = T1.multiply(S).multiply(T2);
				m_Object_OverLay_Hover->setGeometry(geo_hover);
				m_Object_OverLay_Hover->scale.set(2.0f,2.0f,2.0f); 
		/*		m_Object_OverLay_Hover->updateMatrix();
				m_Object_OverLay_Hover->updateMatrixWorld();
				m_Object_OverLay_Hover->updateWorldMatrix(); */
				//geo_hover->applyMatrix4(threepp::Matrix4().makeScale(1.05, 1.05, 1.05));
				/*geo_hover->center();
				geo_hover->scale(2.0f, 2.0f, 2.0f); 
				geo_hover->computeBoundingBox(); 
				geo_hover->computeBoundingSphere(); 
				geo_hover->computeVertexNormals();*/ 
				//m_Object_OverLay_Hover->setGeometry(geo_hover);
				//m_Object_OverLay_Hover->matrix->copy(finalMatrix); 
		/*		m_Object_OverLay_Hover->position.set(center.x,center.y,center.z); 
				m_Object_OverLay_Hover->updateMatrix();
				m_Object_OverLay_Hover->updateMatrixWorld();
				m_Object_OverLay_Hover->updateWorldMatrix();*/
				////m_Object_OverLay_Hover->position.set(center.x,center.y,center.z); 
				//m_Object_OverLay_Hover->updateMatrix(); 
				//m_Object_OverLay_Hover->updateMatrixWorld(); 
			}
			m_Old_ExpressID = m_Current_ExpressID;
		}
	}

	void IFCFileContext::updateCoordHitPoint()
	{
		//m_Hit_Point->geometry()->setFromPoints({ m_Coord_HitPoint });
		if (!m_Coord_HitPoint) return;
		m_Hit_Point->position.set(m_Coord_HitPoint.value().x, m_Coord_HitPoint.value().y, m_Coord_HitPoint.value().z);
	}

	void IFCFileContext::updateCoordAxesHelper()
	{
		if (!m_Coord_HitPoint) return;
		m_Axes_Helper->position.set(m_Coord_HitPoint.value().x, m_Coord_HitPoint.value().y, m_Coord_HitPoint.value().z);
	}

	std::shared_ptr<threepp::Mesh> IFCFileContext::createHoverMesh()
	{
		/*CREATE HOVER MESH FOR MODEL*/
		m_Object_OverLay_Hover = threepp::Mesh::create();
		//m_Object_OverLay_Hover->setMaterial(m_Material_Hover);
		//m_Object_OverLay_Hover->setMaterial(m_Custom_material);
		m_Object_OverLay_Hover->setMaterial(m_Basic_Material);
		m_Object_OverLay_Hover->visible = false;
		return m_Object_OverLay_Hover;
	}

	std::shared_ptr<threepp::Mesh> IFCFileContext::createHitPoint()
	{
		/*HIT POINT LAYER OVERLAY*/
		if (!m_Hit_Point)
		{
			const float sphereRadius = 0.04f;
			const auto sphereGeometry = threepp::SphereGeometry::create(sphereRadius);
			m_Hit_Point = threepp::Mesh::create(sphereGeometry, m_Material_Hit_Point);
		}
		return m_Hit_Point;
	}

	std::shared_ptr<threepp::AxesHelper> IFCFileContext::createAxesHelper()
	{
		m_Axes_Helper = threepp::AxesHelper::create(1.0f);
		m_Axes_Helper->material()->depthTest = false;
		return m_Axes_Helper;
	}

	void IFCFileContext::LButtonUp(EventData& data)
	{
		/*UPDATE COORD HIT POINT CLICKED*/
		if (m_bIsSelectPivotMode)
		{
			updateCoordAxesHelper();
			if (m_Coord_HitPoint)
			{
				auto old_q = data.camera->quaternion;
				data.control->target.set(m_Coord_HitPoint.value().x, m_Coord_HitPoint.value().y, m_Coord_HitPoint.value().z);
				data.control->update();
				data.camera->quaternion = old_q;
				data.camera->updateMatrix();
			}
		}
	}

	void IFCFileContext::LButtonDown(EventData& data)
	{
	}

	void IFCFileContext::RButtonUp(EventData& data)
	{
		int a = 3;
	}

	void IFCFileContext::RButtonDown(EventData& data)
	{
		int a = 3;
	}

	void IFCFileContext::KeyDown(KeyData& data)
	{
	}

	void IFCFileContext::KeyUp(KeyData& data)
	{
	}

	void IFCFileContext::ToolBarAction(ToolBarData& data)
	{
		ID_EVENT id = static_cast<ID_EVENT>(data.event.GetId());
		bool isCheck = data.bIsCheck;
		switch (id)
		{
		case dragon::ID_EVENT::TOOL_PIVOT:
		{
			m_bIsSelectPivotMode = isCheck;
			break;
		}
		case dragon::ID_EVENT::TOOL_HOVER: 
		{
			m_bIsHoverMode = isCheck; 
			break; 
		}
		default:
			break;
		}
	}

	void IFCFileContext::initCallback()
	{
		auto lambda_toggle_component_callback = [&](const std::pair<int, ItemData*>& entity) {
			//spdlog::info("callback ifc file context run : {}", entity.first);
			};
		auto lambda_toggle_componenents_callback = [&](const std::vector<std::pair<int, ItemData*>>& entities) {
			for (auto& [state, ItemData] : entities)
			{
				IFCModelCache::element* ptr_element = static_cast<IFCModelCache::element*>(ItemData->GetData());
				if (ptr_element)
				{
					ptr_element->state = state;
				}
			}
			////rebuild indices
			rebuildVisibleIndices();
			};
		auto lambda_get_item_value_callback = [&](const int& itemId) {
			auto it = m_Model->m_Geometry_Offset.find(itemId);
			if (it != m_Model->m_Geometry_Offset.end())
			{
				void* voidPtr = static_cast<void*>(&it->second);
				return voidPtr;
			}
			else
				return ((void*)0);
			};
		auto lambda_select_object = [&](const unsigned int prim_id)
			{
				const int& index_face_a = 3 * prim_id;
				const int& index_face_b = 3 * prim_id + 1;
				const int& index_face_c = 3 * prim_id + 2;
				const int& a = m_Model->m_Object_Indices[index_face_a];
				const int& expressID = m_Model->m_Object_ExpressID[a];
				auto it = m_Hidden_Express_IDs.find(expressID);
				return it == m_Hidden_Express_IDs.end();
			};
		m_Toggle_Component_Callback = std::make_shared<std::function<void(const std::pair<int, ItemData*>&)>>(lambda_toggle_component_callback);
		m_Toggle_Components_Callback = std::make_shared<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>>(lambda_toggle_componenents_callback);
		m_GetData_Item_Callback = std::make_shared<std::function<void* (const int&)>>(lambda_get_item_value_callback);
		m_Callback_Intersect = std::make_shared<std::function<bool(const unsigned int)>>(lambda_select_object);
	}
}