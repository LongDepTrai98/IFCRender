#include "IFCFileContext.hpp"
#include "IFCGeometryCache.hpp"
#include "threepp/threepp.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include "core/io/factory/GeometryCacheOffsetFactory.hpp"
#include "core/utils/ThreeHelper.hpp"
#include "raycast/CustomRayCaster.hpp"
#include "spdlog/spdlog.h"
#include <format>
#include <iostream>
#include <span>
#include <array>
#include <set>
#include "ui/ElementTreeCtrl.hpp"
namespace dragon
{
	IFCFileContext::IFCFileContext()
	{
		m_Model = std::make_unique<IFCModelCache>();
		/*INIT MATERIAL HOVER*/
		if (!m_Material_Hover)
		{
			m_Material_Hover = threepp::MeshBasicMaterial::create();
			m_Material_Hover->as<threepp::MeshBasicMaterial>()->color = threepp::Color::lightblue;
			m_Material_Hover->transparent = true;
			m_Material_Hover->depthWrite = false;
			m_Material_Hover->depthTest = false;
			m_Material_Hover->opacity = 0.8f;
		}
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
	void IFCFileContext::handleRaycast(CustomRayCaster& RayCaster, threepp::Vector2& nor_mouse_pos)
	{
		if (!m_bIsEnableHover) return;
		if (!m_Model->m_Object_Model)
		{
			CustomRayCaster::Result result;
			bool hit = RayCaster.intersectObjects(result);
			if (hit)
			{
				const int& prim_id = result.prim_id;
				const int& index_face_a = 3 * prim_id;
				const int& index_face_b = 3 * prim_id + 1;
				const int& index_face_c = 3 * prim_id + 2;
				auto obj_geometry = m_Model->m_Object_Model->geometry();
				const int& a = obj_geometry->getIndex()->array()[index_face_a];
				auto attribute_expressid = obj_geometry->getAttribute<unsigned int>("expressID");
				auto& arr = attribute_expressid->array();
				const int& expressID = arr[a];
				if (m_Current_ExpressID != expressID)
				{
					/*UPDATE EXPRESSID*/
					m_Current_ExpressID = expressID;
					spdlog::info("current expressID : {}", m_Current_ExpressID);
					spdlog::info("A : {}, B : {}, C: {}", index_face_a, index_face_b, index_face_c);
				}
			}
			else
			{
				m_Current_ExpressID = -1;
			}
		}
	}
	void IFCFileContext::handleHoverResult(std::shared_ptr<threepp::Mesh>& object_hover)
	{
		if (!m_bIsEnableHover) return;
		if (m_Current_ExpressID != -1)
		{
			if (m_Current_ExpressID = m_Old_ExpressID)
			{
				if (!object_hover->visible)
				{
					object_hover->visible = true;
				}
				/*auto& data_offset = geo_cache->getDataOffset();
				auto it = data_offset.find(m_Current_ExpressID);
				if (it == data_offset.end()) return;
				auto& offsets = it->second;
				std::vector<std::shared_ptr<threepp::BufferGeometry>> geometries{};
				for (auto& offset : offsets)
				{
					geometries.emplace_back(offset.geometry);
				}
				std::shared_ptr<threepp::BufferGeometry> mergeo = threepp::mergeBufferGeometries(geometries, false);
				object_hover->setGeometry(mergeo);
				object_hover->setMaterial(m_Material_Hover);
				m_Old_ExpressID = m_Current_ExpressID;*/
			}
			/*HIDE GEO*/
		}
		else
		{
			object_hover->visible = false;
			m_Old_ExpressID = -1;
		}
	}

	void IFCFileContext::rebuildVisibleIndices(std::unordered_set<uint32_t> set_hides_offset)
	{
		std::vector<int> rebuild_indices{};
		std::shared_ptr<threepp::BufferGeometry> root_geometry = m_Model->m_Object_Model->geometry();
		std::unordered_map<int,std::vector<std::pair<int,int>>> offset_set;
		for (auto& [expressID, offsets] : m_Model->m_Geometry_Offset)
		{
			if (set_hides_offset.find(expressID) == set_hides_offset.end())
			{
				for (auto& offset : offsets)
				{
					int begin_offset = offset.begin_indices_offset;
					int end_offset = offset.end_indices_offset;
					const int& index_material = offset.group.materialIndex; 
					std::pair<int,int> pair = { begin_offset,end_offset }; 
					offset_set[index_material].emplace_back(pair);
				}
			}
		}
		//update indices
		int index_offset{ 0 }; 
		std::vector<threepp::GeometryGroup> groups;
		int start = 0; 
		for (int i = 0; i < m_Model->m_Object_Materials.size(); ++i)
		{
			int count{ 0 }; 
			threepp::GeometryGroup group;
			group.start = start; 
			group.materialIndex = i; 
			auto& set = offset_set[i];
			std::sort(set.begin(), set.end(), [](const auto& a, const auto& b) {
				return a.first < b.first;
				});
			for (auto& [begin, end] : set)
			{
				count += (end - begin) + 1; 
				rebuild_indices.insert(
					rebuild_indices.end(),
					m_Model->m_Object_Indices.begin() + begin,
					m_Model->m_Object_Indices.begin() + end + 1
				);
			}
			group.count = count; 
			start += count; 
			groups.emplace_back(group); 
		}
		/*UPDATE GROUP*/
		root_geometry->clearGroups();
		root_geometry->groups = groups;
		root_geometry->setIndex(rebuild_indices);
	}

	void IFCFileContext::initCallback()
	{
		auto lambda_toggle_component_callback = [&](const std::pair<int, ItemData*>& entity)
			{
				spdlog::info("callback ifc file context run : {}", entity.first);
			};
		auto lambda_toggle_componenents_callback = [&](const std::vector<std::pair<int, ItemData*>>& entities)
			{
				std::unordered_set<uint32_t> set_hide_items{};
				for (auto& [state, item_data] : entities)
				{
					if (!state)
					{
						const int& expressID = *std::get<int*>(item_data->GetData());
						if (m_Model->m_Geometry_Offset.find(expressID) != m_Model->m_Geometry_Offset.end())
						{
							set_hide_items.insert({ (uint32_t)expressID });
						}
					}
				}
				//rebuild indices
				rebuildVisibleIndices(std::move(set_hide_items));
			};
		m_Toggle_Component_Callback = std::make_shared<std::function<void(const std::pair<int, ItemData*>&)>>(lambda_toggle_component_callback);
		m_Toggle_Components_Callback = std::make_shared<std::function<void(const std::vector<std::pair<int, ItemData*>>&)>>(lambda_toggle_componenents_callback);
	}
}