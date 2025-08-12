#include "IFCFileContext.hpp"
#include "IFCGeometryCache.hpp"
#include "threepp/threepp.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include "threepp/geometries/EdgesGeometry.hpp"
#include "core/io/factory/GeometryCacheOffsetFactory.hpp"
#include "raycast/CustomRayCaster.hpp"
#include "spdlog/spdlog.h"
#include "ui/ElementTreeCtrl.hpp"
#include "core/utils/ThreeHelper.hpp"
#include "input/input.hpp"
#include "resource.hpp"
#include "config/app_config.hpp"
#include "threepp/materials/RawShaderMaterial.hpp"
#include <format>
#include <iostream>
#include <set>
#include "core/lock/ContextLock.hpp"
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
		m_Selected_Entites.clear();
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
		CustomRayCaster::Result result;
		bool hit = RayCaster.intersectObjects(result);
		if (hit)
		{
			const int& prim_id = result.prim_id;
			const int& index_face_a = 3 * prim_id;
			const int& index_face_b = 3 * prim_id + 1;
			const int& index_face_c = 3 * prim_id + 2;
			const int& a = m_Model->m_Object_Indices[index_face_a];
			const int& expressID = (*m_Model->ptr_object_expressID)[a];
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
			m_Material_Hit_Point->depthTest = false;
		}

		if (!m_Selected_Material)
		{
			m_Selected_Material = threepp::MeshBasicMaterial::create();
			m_Selected_Material->color = default_color::selected_object_color;
			m_Selected_Material->transparent = true;
			m_Selected_Material->opacity = 0.7f;
			m_Selected_Material->depthTest = false;
		}

		if (!m_Basic_Material)
		{
			m_Basic_Material = threepp::MeshBasicMaterial::create();
			m_Basic_Material->color = threepp::Color::lightgreen;
			m_Basic_Material->side = threepp::Side::Back;
			m_Basic_Material->depthWrite = false;
			m_Basic_Material->polygonOffset = true;
			m_Basic_Material->polygonOffsetFactor = 1;
			m_Basic_Material->polygonOffsetUnits = 1;
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
		if (m_bIsHoverMode)
		{
			updateCoordHitPoint();
			if (OnRedrawCallback)
				OnRedrawCallback();
		}
	}

	void IFCFileContext::rebuildVisibleIndices()
	{
		Context_Lock->lock(); 
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
		auto model = m_Container_Group_Draw->getObjectByName("model");
		/*GET VERTEX ATTRIBUTES*/
		/*GET NORMAL ATTRIBUTES*/
		auto vertices_attribute = model->geometry()->getAttribute<float>("position");
		auto normal_attribute = model->geometry()->getAttribute<float>("normal");
		auto expressID_attribute = model->geometry()->getAttribute<unsigned int>("expressID");
		std::shared_ptr<threepp::BufferGeometry> sub_geometry = ThreeHelper::BuildSubGeometry(total_indices,
			m_Model->m_Object_Materials,
			view_geometries_with_materials,
			m_Model->m_Object_Indices,
			vertices_attribute->array(),
			normal_attribute->array(),
			*model->matrix.get(),
			expressID_attribute->array());
		std::shared_ptr<threepp::Mesh> mesh = threepp::Mesh::create(sub_geometry, m_Model->m_Object_Materials);
		m_Model->ptr_object_expressID = &mesh->geometry()->getAttribute<unsigned int>("expressID")->array();
		m_Model->ptr_object_vertices = mesh->geometry()->getAttribute<float>("position")->array().data();
		m_Container_Group_Draw->clear();
		m_Container_Group_Draw->add(mesh);
		mesh->name = "model";
		RayCast->createNewTriangleIntersect(m_Model->ptr_object_vertices, m_Model->m_Object_Indices.data());
		RayCast->getIntersector()->custom_callback_checkface = m_Callback_Intersect;
		Context_Lock->unlock(); 
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
				/*	IFCModelCache::element& e = it->second;
					std::shared_ptr<threepp::BufferGeometry> geo_hover = ThreeHelper::BuildSubGeometryWithOffset(e,
						m_Model->m_Object_Vertices,
						m_Model->m_Object_Normals,
						m_Model->m_Object_Indices
					);
					threepp::Box3 box;
					box.setFromArray(geo_hover->getAttribute<float>("position")->array());*/
			}
			m_Old_ExpressID = m_Current_ExpressID;
		}
	}

	void IFCFileContext::drawSelectedLayer()
	{
		if (m_Selected_Entites.size() == 0) return;
	}

	void IFCFileContext::updateCoordHitPoint()
	{
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
		m_Object_OverLay_Hover->setMaterial(m_Basic_Material);
		m_Object_OverLay_Hover->visible = false;
		m_Object_OverLay_Hover->matrixAutoUpdate = true;
		m_Object_OverLay_Hover->name = "Hover_Mesh";
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
			m_Hit_Point->name = "Hit_Point";
		}
		return m_Hit_Point;
	}

	std::shared_ptr<threepp::AxesHelper> IFCFileContext::createAxesHelper()
	{
		m_Axes_Helper = threepp::AxesHelper::create(1.0f);
		m_Axes_Helper->material()->depthTest = false;
		m_Axes_Helper->name = "Axes_Helper";
		return m_Axes_Helper;
	}

	std::shared_ptr<threepp::Group> IFCFileContext::createSelectedGroup()
	{
		m_Group_Selected = threepp::Group::create();
		m_Group_Selected->name = "Groupd_Object_Selected";
		return m_Group_Selected;
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
		Context_Lock->lock(); 
		if (m_bIsSelectPivotMode) return;
		if (!m_Current_ExpressID) return;
		if (!m_bIsMultiSelectMode)
		{
			m_Selected_Entites.clear();
			m_Group_Selected->clear();
		}
		m_Selected_Entites.insert(m_Current_ExpressID.value());
		/*CREATE GEO*/
		auto model = m_Container_Group_Draw->getObjectByName("model");
		auto vertices_attribute = model->geometry()->getAttribute<float>("position");
		auto normal_attribute = model->geometry()->getAttribute<float>("normal");
		IFCModelCache::element& e = m_Model->m_Geometry_Offset[m_Current_ExpressID.value()];
		std::shared_ptr<threepp::BufferGeometry> geo_hover = ThreeHelper::BuildSubGeometryWithOffset(e,
			vertices_attribute->array(),
			normal_attribute->array(),
			m_Model->m_Object_Indices
		);
		/*CREATE GEOMEMTRY*/
		std::shared_ptr<threepp::Mesh> mesh_selected = threepp::Mesh::create(geo_hover, m_Selected_Material);
		mesh_selected->name = std::to_string(m_Current_ExpressID.value());
		m_Group_Selected->add(mesh_selected);
		if (m_Add_Object_DrawDepth_CallBack)
		{
			m_Add_Object_DrawDepth_CallBack({ geo_hover }, m_bIsMultiSelectMode);
		}
		if (OnRedrawCallback)
			OnRedrawCallback();
		Context_Lock->unlock(); 
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
		Context_Lock->lock(); 
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
		case dragon::ID_EVENT::TOOL_DRAW_EDGE:
		{
			auto object = m_OverLay_Group->getObjectByName("Outline_Edge");
			if (object)
			{
				object->visible = !object->visible;
				return;
			}
			auto model = m_Container_Group_Draw->getObjectByName("model");
			if (!model) return;
			std::shared_ptr<threepp::EdgesGeometry> edge_geo = threepp::EdgesGeometry::create(*model->geometry(), outline_edge::THRESHOLD_ANGLE);
			std::shared_ptr<threepp::LineBasicMaterial> outline_material = threepp::LineBasicMaterial::create();
			outline_material->color = threepp::Color::darkgray;
			std::shared_ptr<threepp::LineSegments>  outlineEdge = threepp::LineSegments::create(edge_geo, outline_material);
			outlineEdge->name = "Outline_Edge";
			m_OverLay_Group->add(outlineEdge);
			break;
		}
		case dragon::ID_EVENT::TOOL_MULTI_SELECT:
		{
			m_bIsMultiSelectMode = isCheck;
			break;
		}
		default:
			break;
		}
		Context_Lock->unlock(); 
	}

	void IFCFileContext::MenuClick(MenuData& data)
	{
		Context_Lock->lock(); 
		ID_EVENT id = static_cast<ID_EVENT>(data.event_id);
		switch (id)
		{
		case dragon::ID_EVENT::MAIN_MENU_HIDE:
		{
			if (m_Selected_Entites.size() == 0) return;
			for (auto& expressID : m_Selected_Entites)
			{
				auto it = m_Model->m_Geometry_Offset.find(expressID);
				if (it != m_Model->m_Geometry_Offset.end())
				{
					it->second.state = 0;
				}
			}
			m_Current_ExpressID = std::nullopt;
			rebuildVisibleIndices();
			m_Group_Selected->clear();
			m_Selected_Entites.clear();
			if (OnRedrawCallback)
				OnRedrawCallback();
			break;
		}
		default:
			break;
		}
		Context_Lock->unlock(); 
	}

	void IFCFileContext::initCallback()
	{
		auto lambda_toggle_component_callback = [&](const std::pair<int, ItemData*>& entity) {
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
			if (OnRedrawCallback)
				OnRedrawCallback();
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
				const int& expressID = (*m_Model->ptr_object_expressID)[a];
				auto it = m_Hidden_Express_IDs.find(expressID);
				return it == m_Hidden_Express_IDs.end();
			};
		m_Toggle_Component_Callback = std::make_shared<std::function<void(const std::pair<int, ItemData*>&)>>(lambda_toggle_component_callback);
		m_Toggle_Components_Callback = std::make_shared<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>>(lambda_toggle_componenents_callback);
		m_GetData_Item_Callback = std::make_shared<std::function<void* (const int&)>>(lambda_get_item_value_callback);
		m_Callback_Intersect = std::make_shared<std::function<bool(const unsigned int)>>(lambda_select_object);
	}
}