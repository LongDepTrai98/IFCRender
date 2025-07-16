#include "IFCFileContext.hpp"
#include "IFCGeometryCache.hpp"
#include "threepp/threepp.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include "core/io/factory/GeometryCacheOffsetFactory.hpp"
#include "raycast/CustomRayCaster.hpp"
#include "spdlog/spdlog.h"
#include "ui/ElementTreeCtrl.hpp"
#include <format>
#include <iostream>
#include <set>
#include "core/utils/ThreeHelper.hpp"
#include "input/input.hpp"

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
			m_Current_ExpressID = -1;
			return; 
		}
		if (!m_Model->m_Object_Model)
		{
			m_Current_ExpressID = -1;
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

			if (result.P != m_Coord_HitPoint)
			{
				m_Coord_HitPoint = result.P; 
			}

		}
		else
		{
			m_Current_ExpressID = -1; 
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
	}
	void IFCFileContext::handleHoverResult()
	{
		if (!m_bIsEnableHover) return;
		drawHoverLayer(); 
		//Draw hit point 
		drawHitPointLayer(); 
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
		m_Current_ExpressID = -1;
	}

	void IFCFileContext::drawHoverLayer()
	{
		if (m_Current_ExpressID != -1)
		{
			if (m_Current_ExpressID != m_Old_ExpressID)
			{
				spdlog::info("Draw overlay layer : {}", m_Current_ExpressID);
				/*CREATE HOVER OBJECT OVERLAY*/
				if (!m_Object_OverLay_Hover->visible)
				{
					m_Object_OverLay_Hover->visible = true;
				}
				/*GET OFFSET HOVER GEOMETRY*/
				auto it = m_Model->m_Geometry_Offset.find(m_Current_ExpressID);
				if (it != m_Model->m_Geometry_Offset.end())
				{
					const IFCModelCache::element& e = it->second;
					std::shared_ptr<threepp::BufferGeometry> geo_hover = ThreeHelper::BuildSubGeometryWithOffset(e,
						m_Model->m_Object_Vertices,
						m_Model->m_Object_Normals,
						m_Model->m_Object_Indices
					);
					m_Object_OverLay_Hover->setGeometry(geo_hover);
				}
				m_Old_ExpressID = m_Current_ExpressID;
			}
			/*HIDE GEO*/
		}
		else
		{
			m_Object_OverLay_Hover->visible = false;
			m_Old_ExpressID = -1;
		}
	}

	void IFCFileContext::drawHitPointLayer()
	{
		//m_Hit_Point->geometry()->setFromPoints({ m_Coord_HitPoint });
		m_Hit_Point->position.set(m_Coord_HitPoint.x, m_Coord_HitPoint.y, m_Coord_HitPoint.z);
	}

	std::shared_ptr<threepp::Mesh> IFCFileContext::createHoverMesh()
	{
		/*CREATE HOVER MESH FOR MODEL*/
		m_Object_OverLay_Hover = threepp::Mesh::create(); 
		m_Object_OverLay_Hover->setMaterial(m_Material_Hover); 
		m_Object_OverLay_Hover->visible = false; 
		return m_Object_OverLay_Hover; 
	}

	std::shared_ptr<threepp::Mesh> IFCFileContext::createHitPoint()
	{
		/*HIT POINT LAYER OVERLAY*/
		if (!m_Hit_Point)
		{
			const float sphereRadius = 0.1f; 
			const auto sphereGeometry = threepp::SphereGeometry::create(sphereRadius);
			m_Hit_Point = threepp::Mesh::create(sphereGeometry, m_Material_Hit_Point);
		}
		return m_Hit_Point; 
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