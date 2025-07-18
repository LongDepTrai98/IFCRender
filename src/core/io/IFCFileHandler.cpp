#include "IFCFileHandler.hpp"
#include "ui/WindowFrame.hpp"
#include "ui/RenderCanvas.hpp"
#include "renderer/THREEPPRenderer.hpp"
#include "renderer/THREEPPSceneBuilder.hpp"
#include "core/utils/AppHelper.hpp"
#include "threepp/utils/BufferGeometryUtils.hpp"
#include "threepp/geometries/EdgesGeometry.hpp"
#include "threepp/helpers/SpotLightHelper.hpp"
#include "threepp/helpers/DirectionalLightHelper.hpp"
#include "core/convert/WebIFCConverter.hpp"
#include "core/io/factory/FileContextFactory.hpp"
#include "core/io/IFileContext.hpp"
#include "core/io/IFCFileContext.hpp"
#include "core/io/IFCGeometryCache.hpp"
#include "config/app_config.hpp"
#include "web-ifc/parsing/IfcLoader.h"
#include "web-ifc/schema/IfcSchemaManager.h"
#include "web-ifc/schema/ifc-schema.h"
#include "web-ifc/modelmanager/ModelManager.h"
#include "web-ifc/geometry/IfcGeometryProcessor.h"
#include <unordered_set>
#include <spdlog/spdlog.h>
#include "core/props/IFCProperties.hpp"
#include "raycast/CustomRayCaster.hpp"
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
		std::shared_ptr<threepp::Group> container{ nullptr };
		std::shared_ptr<threepp::BufferGeometry> model_geometry{ nullptr };
		WebIFCConverter IFCApi{};
		container = IFCApi.convert(file_path);
		WindowFrame* window_frame = static_cast<WindowFrame*>(m_Window);
		auto main_viewport = AppHelper::getMainViewPortScene(window_frame);
		main_viewport->resetFileContext();
		/*COPY DATA*/
		std::unique_ptr<IFileContext> file_context = FileContextFactory::create(FileContextFactory::type::IFC);
		auto ptr_ifc_file_context = static_cast<IFCFileContext*>(file_context.get());
		auto ptr_ifc_offset_cache = ptr_ifc_file_context->getModelCache();
		ptr_ifc_offset_cache->setModelManager(IFCApi.getModelManager(), IFCApi.getModelId());
		if (file_context)
		{
			main_viewport->setFileContext(std::move(file_context));
			ptr_ifc_offset_cache->m_Object_Model = container->children[0];
			std::shared_ptr<threepp::BufferGeometry> root_geometry = container->children[0]->geometry();
			//ptr_ifc_file_context->setRootObject(group->children[0]);
			ptr_ifc_offset_cache->m_Object_Indices = root_geometry->getIndex()->array();
			ptr_ifc_offset_cache->m_Geometry_Offset = IFCApi.getGeometryOffset();
			threepp::Object3D* model = container->children[0];
			if (!model) return;
			model_geometry = model->geometry();
			if (!root_geometry) return;
			ptr_ifc_offset_cache->m_Object_Materials = model->as<threepp::ObjectWithMaterials>()->materials();
			auto& array_vertices = model_geometry->getAttribute<float>("position")->array();
			ptr_ifc_offset_cache->m_Object_Vertices.assign(array_vertices.begin(), array_vertices.end());
			auto& array_normals = model_geometry->getAttribute<float>("normal")->array();
			ptr_ifc_offset_cache->m_Object_Normals.assign(array_vertices.begin(), array_vertices.end());
			auto& array_expressID = model_geometry->getAttribute<unsigned int>("expressID")->array();
			ptr_ifc_offset_cache->m_Object_ExpressID = array_expressID;
			/*INDEX MODEL IS 0*/
		}

		if (m_Window)
		{
			/*GET MAIN VIEWPORT AND BUILD MAIN SCENE*/
			WindowFrame* window_frame = static_cast<WindowFrame*>(m_Window);
			auto main_viewport = AppHelper::getMainViewPortScene(window_frame);
			if (main_viewport)
			{
				main_viewport->clearScene();
				auto viewport_scene = main_viewport->getScene();
				auto camera = main_viewport->getCamera();
				viewport_scene->add(container);
				/*CREATE OUTLINE EDGE INDEX IS 1*/
				std::shared_ptr<threepp::EdgesGeometry> edge_geo = threepp::EdgesGeometry::create(*model_geometry, outline_edge::THRESHOLD_ANGLE);
				std::shared_ptr<threepp::LineBasicMaterial> outline_material = threepp::LineBasicMaterial::create();
				outline_material->linewidth = 100.0f;
				outline_material->color = threepp::Color::darkgray;
				std::shared_ptr<threepp::LineSegments>  outlineEdge = threepp::LineSegments::create(edge_geo, outline_material);
				viewport_scene->add(outlineEdge);
				/*CREATE OVERLAY LAYER INDEX IS 2*/
				std::shared_ptr<threepp::Group> overlay_group = threepp::Group::create();
				overlay_group->name = "Overlay";
				overlay_group->add(ptr_ifc_file_context->createHoverMesh());
				overlay_group->add(ptr_ifc_file_context->createHitPoint());
				overlay_group->add(ptr_ifc_file_context->createAxesHelper());
				viewport_scene->add(overlay_group);
				//main_viewport->initObjectHover();
				/*CREATE */
				SceneBuilder::IFCBuildScene(container.get(), viewport_scene, camera);
			}

			/*CREATE ELEMENT TREE AND BIND FUNC*/
			auto element_tree = AppHelper::getMainTreeCtrl(window_frame);
			if (element_tree)
			{
				IFCProperties build_props(IFCApi.getModelId(), IFCApi.getModelManager().get());
				auto tree = build_props.createTreeNode(IFCApi.getModelId());
				//create callback for tree
				ptr_ifc_file_context->initCallback();
				/*BIND CALLBACK*/
				element_tree->m_ToggleStateCallBack = ptr_ifc_file_context->m_Toggle_Component_Callback;
				element_tree->m_ToggleStateCallBackRecursively = ptr_ifc_file_context->m_Toggle_Components_Callback;
				element_tree->m_GetData_Item_Callback = ptr_ifc_file_context->m_GetData_Item_Callback;
				element_tree->setData(std::move(tree));
			}

			/*SET CALLBACK RAYCAST*/
			main_viewport->buildBVH(ptr_ifc_offset_cache->m_Object_Vertices, ptr_ifc_offset_cache->m_Object_Indices);
			auto RayCast = main_viewport->getRayCaster();
			ptr_ifc_file_context->RayCast = RayCast;
			RayCast->getIntersector()->custom_callback_checkface = ptr_ifc_file_context->m_Callback_Intersect;
		}
		RenderCanvas* canvas = AppHelper::getRenderCanvas(window_frame);
		/*set callback redraw */
		auto redraw_callback = [canvas]() {
			canvas->Invalidate();
			};
		ptr_ifc_file_context->OnRedrawCallback = redraw_callback;
		canvas->Invalidate();
	}
}