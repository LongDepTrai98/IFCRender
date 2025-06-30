#include "IFCFileContext.hpp"
#include "IFCGeometryCache.hpp"
#include "threepp/threepp.hpp"
#include "core/io/factory/GeometryCacheOffsetFactory.hpp"
#include <format>
#include <iostream>
namespace dragon
{
	IFCFileContext::IFCFileContext()
	{
		m_Geometry_Offset_Cache = std::move(GeometryCacheOffsetFactory::create(GeometryCacheOffsetFactory::TYPE::IFC)); 
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
					const int& a = intersect.face.value().a;
					const int& b = intersect.face.value().b;
					const int& c = intersect.face.value().c;
					auto geo = m_Children_Objects[0]->geometry();
					auto attribute = geo->getAttribute<unsigned int>("expressID");
					auto& arr = attribute->array();
					const int& expressIDA = arr[a];
					const int& expressIDB = arr[a];
					const int& expressIDC = arr[a];
					if (expressIDA != expressIDB
						|| expressIDB != expressIDC
						|| expressIDA != expressIDC)
					{
						std::cout << std::format("expressID A: {} , B : {} , C : {}", expressIDA, expressIDB, expressIDC);
					}
					std::cout << std::format("expressID A: {} , B : {} , C : {}", expressIDA, expressIDB, expressIDC) << std::endl;
				}
			}
		}
	}
	void IFCFileContext::setRootObject(threepp::Object3D* root_mesh)
	{
		std::vector<threepp::Object3D*> lstObject{};
		lstObject.emplace_back(root_mesh); 
		m_Children_Objects = std::move(lstObject); 
	}
}