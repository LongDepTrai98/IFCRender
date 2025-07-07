#include "WebIFCHelper.hpp"
namespace dragon
{
	nlohmann::json WebIFCHelper::ReadValue(webifc::manager::ModelManager& manager, uint32_t modelID, webifc::parsing::IfcTokenType t)
	{
	    auto loader = manager.GetIfcLoader(modelID);
        switch (t)
        {
        case webifc::parsing::IfcTokenType::STRING:
        {
            return loader->GetDecodedStringArgument(); 
        }
        case webifc::parsing::IfcTokenType::ENUM:
        {
            std::string_view s = loader->GetStringArgument();
            if (s == "T") {
                return true; 
            }
            if (s == "F") {
                return false; 
            }
            if (s == "U") {
                return nullptr; 
            }
            return std::string(s); 
        }
        case webifc::parsing::IfcTokenType::REAL:
        { 
            return loader->GetDoubleArgument(); 
        }
        case webifc::parsing::IfcTokenType::INTEGER:
        {
			return loader->GetIntArgument();
         
        }
        case webifc::parsing::IfcTokenType::REF:
        {
            return loader->GetRefArgument(); 
        }
        default:
            // use undefined to signal val parse issue
            return nullptr; 
        }
    }
    nlohmann::json WebIFCHelper::GetArgs(webifc::manager::ModelManager& manager, uint32_t modelID, bool inObject, bool inList)
    {
        auto loader = manager.GetIfcLoader(modelID);
        //auto arguments = emscripten::val::array();
        auto arguments = nlohmann::json::array(); 
        bool endOfLine = false;
        while (!loader->IsAtEnd() && !endOfLine)
        {
            webifc::parsing::IfcTokenType t = loader->GetTokenType();
            switch (t)
            {
            case webifc::parsing::IfcTokenType::LINE_END:
            {
                endOfLine = true;
                break;
            }
            case webifc::parsing::IfcTokenType::EMPTY:
            {
                //arguments.set(size++, emscripten::val::null());
                arguments.push_back(nullptr); 
                break;
            }
            case webifc::parsing::IfcTokenType::SET_BEGIN:
            {
                //arguments.set(size++, GetArgs(modelID, false, true));
                arguments.push_back(GetArgs(manager, modelID, false, true)); 
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
                //auto obj = emscripten::val::object();
				auto obj = nlohmann::json::object();
				obj["type"] = static_cast<uint32_t>(webifc::parsing::IfcTokenType::LABEL);
                //obj.set("type", emscripten::val(static_cast<uint32_t>(webifc::parsing::IfcTokenType::LABEL)));
                loader->StepBack();
                auto s = loader->GetStringArgument();
                auto typeCode = manager.GetSchemaManager().IfcTypeToTypeCode(s);
                //obj.set("typecode", emscripten::val(typeCode));
                obj["typecode"] = typeCode;
                // read set open
                loader->GetTokenType();
                //obj.set("value", GetArgs(modelID, true));
				obj["value"] = GetArgs(manager, modelID, true);
                //arguments.set(size++, obj);
				arguments.push_back(obj);
                break;
            }
            case webifc::parsing::IfcTokenType::STRING:
            case webifc::parsing::IfcTokenType::ENUM:
            case webifc::parsing::IfcTokenType::REAL:
            case webifc::parsing::IfcTokenType::INTEGER:
            case webifc::parsing::IfcTokenType::REF:
            {
                loader->StepBack();
                //emscripten::val obj;
                 //obj = nlohmann::json::object;
                if (inObject)
                {
                    nlohmann::json vartiant_value{};
                    vartiant_value = ReadValue(manager, modelID, t);
					arguments.push_back(!vartiant_value.is_null() ? vartiant_value : nullptr);
                }
                else {
                    //obj = emscripten::val::object();
                    auto obj = nlohmann::json::object(); 
					obj["type"] = static_cast<uint32_t>(t);
                    obj["value"] = ReadValue(manager, modelID, t);
                    arguments.push_back(obj); 
                    //obj.set("type", emscripten::val(static_cast<uint32_t>(t)));
                    //obj.set("value", ReadValue(modelID, t));
                }
                break;
            }
            default:
                break;
            }
        }
        if (arguments.size() == 0 && !inList) return nullptr;
        if (arguments.size() == 1 && inObject) return arguments[0];
        return arguments;
    }
    nlohmann::json WebIFCHelper::GetLine(webifc::manager::ModelManager& manager,
        const uint32_t& modelID,
        const uint32_t& expressID,
        bool flatten,
        bool inverse)
    {
        auto loader = manager.GetIfcLoader(modelID);
        if (!manager.IsModelOpen(modelID)) return nullptr; 
        if (!loader->IsValidExpressID(expressID)) return nullptr; 
        uint32_t lineType = loader->GetLineType(expressID);
        if (lineType == 0) return nullptr; 
        loader->MoveToArgumentOffset(expressID, 0);
        nlohmann::json data; 
        data["ID"] = expressID; 
        data["type"] = lineType; 
        data["arguments"] = GetArgs(manager, modelID);
        return data;
    }
    std::vector<uint32_t> WebIFCHelper::GetLineIDsWithType(webifc::manager::ModelManager& manager, const uint32_t& modelID, const uint32_t& expressID)
    {
        if (!manager.IsModelOpen(modelID)) return {};
        auto loader = manager.GetIfcLoader(modelID);
        std::vector<uint32_t> expressIDs;
        auto ids = loader->GetExpressIDsWithType(expressID);
        expressIDs.insert(expressIDs.end(), ids.begin(), ids.end());
        return expressIDs;
    }
    nlohmann::json WebIFCHelper::GetLineFromRawLine(const nlohmann::json& RawLine)
    {
        nlohmann::json line; 
        const uint32_t& type = RawLine["type"].get<uint32_t>();
        if (type == webifc::schema::IFCRELAGGREGATES)
        {
            /*aggregates*/
            auto& relating = RawLine["arguments"][4];
            if (relating.is_object())
            {
                const uint32_t& lineID = relating["value"].get<uint32_t>();
                line["relating"] = lineID; 
            }
            auto& related = RawLine["arguments"][5];
            if (related.is_array())
            {
                for (auto& lineOBJ : related)
                {
                    const uint32_t& lineID = lineOBJ["value"].get<uint32_t>(); 
                    line["related"].push_back(lineID); 
                }
            }
        } else if (type == webifc::schema::IFCRELCONTAINEDINSPATIALSTRUCTURE)
        {
            /*spatial*/
            auto& relating = RawLine["arguments"][5]; 
            if (relating.is_object())
            {
                const uint32_t& lineID = relating["value"].get<uint32_t>();
                line["relating"] = lineID;
            }
            auto& related = RawLine["arguments"][4]; 
            if (related.is_array())
            {
                for (auto& lineOBJ : related)
                {
                    const uint32_t& lineID = lineOBJ["value"].get<uint32_t>();
                    line["related"].push_back(lineID);
                }
            }
        }
        else
        {
            if (!RawLine["arguments"][2].is_null())
            {
                if (RawLine["arguments"][2]["type"] == 1)
                {
                    if (!RawLine["arguments"][2]["value"].is_null())
                    {
                        std::string name = RawLine["arguments"][2]["value"];
                        line["name"] = name;
                    }
                }
            }
            else
            {
                std::cout << RawLine.dump() << std::endl; 
                line["name"] = RawLine["arguments"][0]["value"].get<std::string>();
            }
        }
        return line; 
    }
}