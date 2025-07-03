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
//#include "core/utils/WebIFCHelper.hpp"
#include <unordered_set>
#include <spdlog/spdlog.h>


std::string ReadValue(webifc::parsing::IfcLoader& loader, webifc::parsing::IfcTokenType t)
{
    switch (t)
    {
    case webifc::parsing::IfcTokenType::STRING:
    {
        return loader.GetDecodedStringArgument();
    }
    case webifc::parsing::IfcTokenType::ENUM:
    {
        std::string_view s = loader.GetStringArgument();
        return std::string(s);
    }
    case webifc::parsing::IfcTokenType::REAL:
    {
        std::string_view s = loader.GetDoubleArgumentAsString();
        return std::string(s);
    }
    case webifc::parsing::IfcTokenType::INTEGER:
    {
        long d = loader.GetIntArgument();
        return std::to_string(d);
    }
    case webifc::parsing::IfcTokenType::REF:
    {
        uint32_t ref = loader.GetRefArgument();
        return std::to_string(ref);
    }
    default:
        // use undefined to signal val parse issue
        return "";
    }
}

std::string GetArgs(webifc::parsing::IfcLoader& loader, bool inObject = false, bool inList = false)
{
    std::string arguments;
    size_t size = 0;
    bool endOfLine = false;
    while (!loader.IsAtEnd() && !endOfLine)
    {
        webifc::parsing::IfcTokenType t = loader.GetTokenType();

        switch (t)
        {
        case webifc::parsing::IfcTokenType::LINE_END:
        {
            endOfLine = true;
            break;
        }
        case webifc::parsing::IfcTokenType::EMPTY:
        {
            arguments += " Empty ";
            break;
        }
        case webifc::parsing::IfcTokenType::SET_BEGIN:
        {
            arguments += GetArgs(loader, false, true);
            break;
        }
        case webifc::parsing::IfcTokenType::SET_END:
        {
            endOfLine = true;
            break;
        }
        case webifc::parsing::IfcTokenType::LABEL:
        {
            // read label
            std::string obj;
            obj = " type: LABEL ";
            loader.StepBack();
            auto s = loader.GetStringArgument();
            // read set open
            loader.GetTokenType();
            obj += " value " + GetArgs(loader, true) + " ";
            arguments += obj;
            break;
        }
        case webifc::parsing::IfcTokenType::STRING:
        case webifc::parsing::IfcTokenType::ENUM:
        case webifc::parsing::IfcTokenType::REAL:
        case webifc::parsing::IfcTokenType::INTEGER:
        case webifc::parsing::IfcTokenType::REF:
        {
            loader.StepBack();
            std::string obj;
            if (inObject)
                obj = ReadValue(loader, t);
            else
            {
                //std::string obj;
                obj += " type REF ";
                obj += ReadValue(loader, t) + " ";
            }
            arguments += obj;
            break;
        }
        default:
            break;
        }
    }
    return arguments;
}

std::string GetLine(webifc::parsing::IfcLoader& loader, uint32_t expressID)
{
    if (!loader.IsValidExpressID(expressID))
        return "";
    uint32_t lineType = loader.GetLineType(expressID);
    if (lineType == 0)
        return "";

    loader.MoveToArgumentOffset(expressID, 0);

    auto arguments = GetArgs(loader);

    std::string retVal;
    retVal += "\"ID\": " + std::to_string(expressID) + ", ";
    retVal += "\"type\": " + std::to_string(lineType) + ", ";
    retVal += "\"arguments\": " + arguments;
    retVal += "}";

    return retVal;
}


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
		//auto& geometryOffsetCache = IFCApi.getGeometryOffsetCache();
		WindowFrame* window_frame = static_cast<WindowFrame*>(m_Window);
		auto main_viewport = AppHelper::getMainViewPortScene(window_frame);
		main_viewport->resetFileContext();
		/*COPY DATA*/
		std::unique_ptr<IFileContext> file_context = FileContextFactory::create(FileContextFactory::type::IFC);
		auto ptr_ifc_file_context = static_cast<IFCFileContext*>(file_context.get());
		auto ptr_ifc_offset_cache = static_cast<IFCGeometryCache*>(file_context->getGeometryCache());


		{
			/*TEST API IFC C++*/
			auto manager = IFCApi.getModelManager(); 
			uint32_t modelID = IFCApi.getModelId(); 
			auto loader = manager->GetIfcLoader(modelID);
			std::vector<uint32_t> expressIDs;

			auto ids = loader->GetExpressIDsWithType(webifc::schema::IFCRELAGGREGATES);
			for (int i = 0; i < ids.size(); ++i)
			{
				auto lineType = loader->GetLineType(ids[i]);
				if (lineType == 0) return; 
				auto t = GetLine(*loader, ids[i]);
                spdlog::info(t);
                int a = 3; 
				/*loader->MoveToArgumentOffset(ids[i], 0);
                auto json = WebIFCHelper::GetArgs(*manager, modelID, false, false);
				spdlog::info(json.dump()); */
			}


		}

		ptr_ifc_offset_cache->setModelManager(IFCApi.getModelManager(), IFCApi.getModelId());
		//ptr_ifc_offset_cache->copyData(IFCApi.getGeometryOffsetCache());
		if (file_context)
		{
			main_viewport->setFileContext(std::move(file_context));
			ptr_ifc_file_context->setRootObject(group->children[0]);
		}
		main_viewport->clearScene();
		main_viewport->buildBVH(group->children[0]->geometry().get());
		if (m_Window)
		{
			/*GET MAIN VIEWPORT*/
			WindowFrame* window_frame = dynamic_cast<WindowFrame*>(m_Window);
			auto main_viewport = AppHelper::getMainViewPortScene(window_frame);
			if (main_viewport)
			{
				auto viewport_scene = main_viewport->getScene();
				auto camera = main_viewport->getCamera();
				viewport_scene->children;
				viewport_scene->add(group);
				main_viewport->initObjectHover();
				SceneBuilder::IFCBuildScene(group.get(), viewport_scene, camera);
			}
		}
	}
}