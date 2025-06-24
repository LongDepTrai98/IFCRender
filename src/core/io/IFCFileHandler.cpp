#include "IFCFileHandler.hpp"
#include "IFCMessageHandler.hpp"


#include "ui/WindowFrame.hpp"
#include "ui/RenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "view/MainViewPort.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"

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
#include "threepp/helpers/SpotLightHelper.hpp"
#include "threepp/helpers/DirectionalLightHelper.hpp"


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
		m_GeometryConverter->setCsgEps(m_Eps);
		/*adjust epsilon for boolean operations*/
		//m_GeometryConverter->setCsgEps(m_Eps); 
#ifdef _DEBUG
		GeomDebugDump::clearMeshsetDump();
#endif
		std::cout << "Converting IFC geometry: ";
		m_GeometryConverter->convertGeometry();
		/*CONVERT ENTITY TO SCENE*/
		IFCConverter converter{};
		std::shared_ptr<threepp::Group> group{ nullptr }; 
		if (!m_bIsCreateInstance)
		{
			group = converter.convert(m_GeometryConverter, m_GeometrySettings);
		}
		else if (m_bIsCreateInstance) {
			group = threepp::Group::create(); 
			converter.convertWithInstancing(m_GeometryConverter, m_GeometrySettings);
			auto shapes = converter.getAllGeo();
			auto material = threepp::MeshBasicMaterial::create();
			material->as<threepp::MeshBasicMaterial>()->color = threepp::Color::lightgray;
			for (auto& [id_entity, lstBuffer] : shapes)
			{
				auto buffer = threepp::mergeBufferGeometries(lstBuffer.lstBuffGeo); 
				material->transparent = true; 
				material->opacity = 0.7f; 
				auto mesh = threepp::Mesh::create(buffer, material); 
				group->add(mesh); 
			}
		}

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
						group->rotation.x = -threepp::math::PI / 2;
						viewport_scene->add(group); 
						threepp::Box3 box{}; 
						box.setFromObject(*group);
						auto center = box.getCenter(); 
						camera->lookAt(center);
						addLight(*viewport_scene,
							group); 
						//createPlane(*viewport_scene); 
					}
				}
			}
		}

	}
	std::shared_ptr<GeometryConverter>& IFCFileHandler::getGeometryConverter()
	{
		return m_GeometryConverter; 
	}
	void IFCFileHandler::addLight(threepp::Scene& scene,
		std::shared_ptr<threepp::Group>& container)
	{
		//std::shared_ptr<threepp::DirectionalLight> d_light_1 = threepp::DirectionalLight::create(threepp::Color::white);
		//std::shared_ptr<threepp::DirectionalLight> d_light_2 = threepp::DirectionalLight::create(threepp::Color::white);
		//std::shared_ptr<threepp::DirectionalLight> d_light_3 = threepp::DirectionalLight::create(threepp::Color::white, 0.6f);
		std::shared_ptr<threepp::AmbientLight> a_light = threepp::AmbientLight::create((threepp::Color::white,0.5f));
		scene.add(a_light);

		//d_light_1->position.set(100, 50, -100); 
		//d_light_2->position.set(-100, 50, 0);
		//d_light_3->position.set(100, 20, 0); 

		//a_light->position.set(0,100,0); 

		//d_light_1->setTarget(*container); 
		//d_light_2->setTarget(*container); 
		//d_light_3->setTarget(*container); 



		//auto helper_d_light_1 = threepp::DirectionalLightHelper::create(*d_light_1, 1.0f, threepp::Color::lightyellow);
		//auto helper_d_light_2 = threepp::DirectionalLightHelper::create(*d_light_2, 1.0f, threepp::Color::red);
		//auto helper_d_light_3 = threepp::DirectionalLightHelper::create(*d_light_3, 1.0f, threepp::Color::blue);
		//
		//scene.add(helper_d_light_1); 
		//scene.add(helper_d_light_2); 
		//scene.add(helper_d_light_3); 
	}
	void IFCFileHandler::createPlane(threepp::Scene& scene)
	{
		auto geometry = threepp::PlaneGeometry::create(150, 150);
		auto material = threepp::MeshBasicMaterial::create(); 
		material->opacity = 0.2f; 
		material->transparent = true; 
		auto mesh = threepp::Mesh::create(geometry, material);
		mesh->rotation.x = -threepp::math::PI / 2;
		auto grid = threepp::GridHelper::create(150, 150);
		grid->rotation.x = threepp::math::PI / 2;
		grid->material()->opacity = 0.3f;
		grid->material()->transparent = true;
		mesh->add(grid);
		scene.add(mesh); 
	}
}