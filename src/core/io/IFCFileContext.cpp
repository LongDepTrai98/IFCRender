#include "IFCFileContext.hpp"
#include "IFCGeometryCache.hpp"
#include "threepp/threepp.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include "core/io/factory/GeometryCacheOffsetFactory.hpp"
#include "core/utils/ThreeHelper.hpp"
#include <format>
#include <iostream>
#include <span>
#include <array>
#define NANORT_USE_CPP11_FEATURE 1
#include "geometry/nanort.hpp"
namespace dragon
{
	IFCFileContext::IFCFileContext()
	{
		m_Geometry_Offset_Cache = std::move(GeometryCacheOffsetFactory::create(GeometryCacheOffsetFactory::TYPE::IFC));
		/*INIT MATERIAL HOVER*/
		if (!m_Material_Hover)
		{
			m_Material_Hover = threepp::MeshBasicMaterial::create();
			m_Material_Hover->as<threepp::MeshBasicMaterial>()->color = threepp::Color::lightblue;
			m_Material_Hover->transparent = true;
			m_Material_Hover->depthWrite = false;
			m_Material_Hover->opacity = 0.8f;
		}
	}
	IFCFileContext::~IFCFileContext()
	{
		m_Children_Objects.clear();
	}
	std::string IFCFileContext::getFileType()
	{
		return std::string("IFC");
	}
	IGeometryCache* IFCFileContext::getGeometryCache()
	{
		return m_Geometry_Offset_Cache.get();
	}
	void IFCFileContext::handleRaycast(threepp::Raycaster& RayCaster, threepp::Vector2& nor_mouse_pos)
	{
		if (!m_Children_Objects.empty())
		{
			const auto intersects = RayCaster.intersectObjects(m_Children_Objects, true);
			if (!intersects.empty())
			{
				const auto& intersect = intersects.front();
				if (intersect.face)
				{
					if (intersect.face)
					{
						const int& a = intersect.face.value().a;
						auto root_object = m_Children_Objects[0]->geometry();
						auto attribute_expressid = root_object->getAttribute<unsigned int>("expressID");
						auto& arr = attribute_expressid->array();
						const int& expressID = arr[a];
						if (m_Current_ExpressID != expressID)
						{
							/*UPDATE EXPRESSID*/
							m_Current_ExpressID = expressID;
						}
					}
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
		if (m_Current_ExpressID != -1)
		{
			if (m_Current_ExpressID != m_Old_ExpressID)
			{
				if (!object_hover->visible)
				{
					object_hover->visible = true;
				}
				IFCGeometryCache* geo_cache = static_cast<IFCGeometryCache*>(m_Geometry_Offset_Cache.get());
				if (!geo_cache) return;
				auto& data_offset = geo_cache->getDataOffset();
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
				m_Old_ExpressID = m_Current_ExpressID;
			}
			/*HIDE GEO*/
		}
		else
		{
			object_hover->visible = false;
			m_Old_ExpressID = -1;
		}
	}
	void IFCFileContext::setRootObject(threepp::Object3D* root_mesh)
	{
		std::vector<threepp::Object3D*> lstObject{};
		lstObject.emplace_back(root_mesh);
		m_Children_Objects = std::move(lstObject);
	}
}