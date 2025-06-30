#include "IFCFileHandler.hpp"
#include "IFCMessageHandler.hpp"
#include "ui/WindowFrame.hpp"
#include "ui/RenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "renderer/THREEPPSceneBuilder.hpp"
#include "core/utils/AppHelper.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include "threepp/geometries/EdgesGeometry.hpp"
#include "core/convert/IFCEntityConvert.hpp"
#include "renderer/THREEPPSceneBuilder.hpp"
#include <unordered_set>
#include <ifcpp/IFC4X3/include/IfcBuildingStorey.h>
#include <ifcpp/IFC4X3/include/IfcGloballyUniqueId.h>
#include <ifcpp/IFC4X3/include/IfcLabel.h>
#include <ifcpp/IFC4X3/include/IfcObjectDefinition.h>
#include <ifcpp/IFC4X3/include/IfcProject.h>
#include <ifcpp/IFC4X3/include/IfcRelAggregates.h>
#include <ifcpp/IFC4X3/include/IfcRelContainedInSpatialStructure.h>
#include <ifcpp/IFC4X3/include/IfcText.h>
#include <ifcpp/model/BuildingModel.h>
#include <ifcpp/reader/ReaderSTEP.h>
#include <ifcpp/geometry/GeometryConverter.h>
#include "threepp/helpers/SpotLightHelper.hpp"
#include "threepp/helpers/DirectionalLightHelper.hpp"
#include "core/io/WebIFCConverter.hpp"
#include "core/io/factory/GeometryCacheOffsetFactory.hpp"

namespace dragon
{
	IFCFileHandler::IFCFileHandler()
	{
	}
	IFCFileHandler::~IFCFileHandler()
	{
	}
	void IFCFileHandler::open(const std::filesystem::path& file_path)
	{
		std::shared_ptr<threepp::Group> group{ nullptr };

		if (m_bIsUseWebIFCConvert)
		{
			WebIFCConverter IFCApi{};
			group = IFCApi.convert(file_path);
			auto& geometryOffsetCache = IFCApi.getGeometryOffsetCache(); 
			WindowFrame* window_frame = dynamic_cast<WindowFrame*>(m_Window);
			auto main_viewport = AppHelper::getMainViewPortScene(window_frame);
			auto cache = GeometryCacheOffsetFactory::create(GeometryCacheOffsetFactory::IFC); 
			auto ptr_ifc_cache = dynamic_cast<IFCGeometryCache*>(cache.get()); 
			if (ptr_ifc_cache)
			{
				ptr_ifc_cache->copyData(IFCApi.getGeometryOffsetCache()); 
			}
			main_viewport->setGeometryOffsetCache(std::move(cache));
		}
		else
		{
			shared_ptr<BuildingModel> ifc_model(new BuildingModel());
			/*CALLBACK MESSAGE HANDLER*/
			IFCMessageHandler mh;
			/*CRATE READER STEP*/
			shared_ptr<ReaderSTEP> step_reader(new ReaderSTEP());
			step_reader->setMessageCallBack(std::bind(&IFCMessageHandler::slotMessageWrapper, &mh, std::placeholders::_1));
			/*LOAD MODEL*/
			step_reader->loadModelFromFile(file_path.string(), ifc_model);
			/*CONVERT MODEL TO GEOMETRY*/
			m_GeometrySettings = std::make_shared<GeometrySettings>();
			m_GeometryConverter = std::make_shared<GeometryConverter>(ifc_model, m_GeometrySettings);
			m_GeometryConverter->setCsgEps(m_Eps);
#ifdef _DEBUG
			GeomDebugDump::clearMeshsetDump();
#endif
			std::cout << "Converting IFC geometry: ";
			m_GeometryConverter->convertGeometry();
			/*CONVERT ENTITY TO SCENE*/
			IFCConverter converter{};
			IFCConverter::MODE mode;
			m_bIsCreateInstance ? mode = IFCConverter::MODE::INSTANCING : mode = IFCConverter::MODE::MESH;
			group = converter.convert(m_GeometryConverter, m_GeometrySettings, mode);
		}
		if (m_Window)
		{
			/*GET MAIN VIEWPORT*/
			WindowFrame* window_frame = dynamic_cast<WindowFrame*>(m_Window);
			auto main_viewport = AppHelper::getMainViewPortScene(window_frame);
			if (main_viewport)
			{
				auto viewport_scene = main_viewport->getScene();
				auto camera = main_viewport->getCamera();
				viewport_scene->clear();
				viewport_scene->children; 
				//group->rotation.x = -threepp::math::PI / 2;
				viewport_scene->add(group);
				SceneBuilder::IFCBuildScene(group.get(), viewport_scene, camera);
			}
		}
	}
	std::shared_ptr<GeometryConverter>& IFCFileHandler::getGeometryConverter()
	{
		return m_GeometryConverter;
	}
}