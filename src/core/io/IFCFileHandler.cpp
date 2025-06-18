#include "IFCFileHandler.hpp"
#include "IFCMessageHandler.hpp"


#include "ui/WindowFrame.hpp"
#include "ui/RenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "view/MainViewPort.hpp"

#include "core/convert/IFCEntityConvert.hpp"
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
		/*adjust epsilon for boolean operations*/
		//m_GeometryConverter->setCsgEps(m_Eps); 
#ifdef _DEBUG
		GeomDebugDump::clearMeshsetDump();
#endif
		std::cout << "Converting IFC geometry: ";
		m_GeometryConverter->convertGeometry();
		/*CONVERT ENTITY TO SCENE*/
		IFCConverter converter{};
		std::shared_ptr<threepp::Group> group =  converter.convert(m_GeometryConverter);
		if (m_Window)
		{
			/*GET MAIN VIEWPORT*/
			WindowFrame* window_frame = dynamic_cast<WindowFrame*>(m_Window); 
			if (window_frame)
			{
				IRenderer* renderer = window_frame->getRenderCanvas()->getRenderer(); 
				/*CONVERT TO THREEPP*/
				THREEPPRenderer* three_renderer = dynamic_cast<THREEPPRenderer*>(renderer); 
				if (three_renderer)
				{
					MainViewPort* m_viewport = dynamic_cast<MainViewPort*>(three_renderer->getMainViewPort());
					if (m_viewport)
					{
						auto viewport_scene = m_viewport->getScene(); 
						auto camera = m_viewport->getCamera(); 
						viewport_scene->clear();
						viewport_scene->add(group); 
						camera->lookAt(group->position);
						m_viewport->addLight(); 
					}
				}
			}
		}

	}
	std::shared_ptr<GeometryConverter>& IFCFileHandler::getGeometryConverter()
	{
		return m_GeometryConverter; 
	}
}