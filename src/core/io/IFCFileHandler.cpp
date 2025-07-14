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
		std::shared_ptr<threepp::Group> group{ nullptr };
		WebIFCConverter IFCApi{};
		group = IFCApi.convert(file_path);
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
			ptr_ifc_offset_cache->m_Object_Model = group->children[0];
			std::shared_ptr<threepp::BufferGeometry> root_geometry = group->children[0]->geometry();
			//ptr_ifc_file_context->setRootObject(group->children[0]);
			ptr_ifc_offset_cache->m_Object_Indices = root_geometry->getIndex()->array();
			ptr_ifc_offset_cache->m_Geometry_Offset = IFCApi.getGeometryOffset();
			ptr_ifc_offset_cache->m_Object_Materials = group->children[0]->as<threepp::ObjectWithMaterials>()->materials();
			auto& array_vertices = group->children[0]->geometry()->getAttribute<float>("position")->array();
			ptr_ifc_offset_cache->m_Object_Vertices.assign(array_vertices.begin(), array_vertices.end());
			auto& array_normals = group->children[0]->geometry()->getAttribute<float>("normal")->array();
			ptr_ifc_offset_cache->m_Object_Normals.assign(array_vertices.begin(), array_vertices.end());
			auto& array_expressID = group->children[0]->geometry()->getAttribute<unsigned int>("expressID")->array();
			ptr_ifc_offset_cache->m_Object_ExpressID = array_expressID; 
		}
		if (m_Window)
		{
			/*GET MAIN VIEWPORT*/
			WindowFrame* window_frame = dynamic_cast<WindowFrame*>(m_Window);
			auto main_viewport = AppHelper::getMainViewPortScene(window_frame);
			if (main_viewport)
			{
				main_viewport->clearScene();
				auto viewport_scene = main_viewport->getScene();
				auto camera = main_viewport->getCamera();
				viewport_scene->children;
				viewport_scene->add(group);
				main_viewport->initObjectHover();
				SceneBuilder::IFCBuildScene(group.get(), viewport_scene, camera);
			}
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

			main_viewport->buildBVH(ptr_ifc_offset_cache->m_Object_Vertices, ptr_ifc_offset_cache->m_Object_Indices);
			auto RayCast = main_viewport->getRayCaster();
			ptr_ifc_file_context->RayCast = RayCast;
			RayCast->getIntersector()->custom_callback_checkface = ptr_ifc_file_context->m_callback_intersect; 
		}
	}
}