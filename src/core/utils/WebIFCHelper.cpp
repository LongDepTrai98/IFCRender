#include "WebIFCHelper.hpp"
using json = nlohmann::json;


static json ReadValue(uint32_t modelID,
	webifc::parsing::IfcTokenType t,
	webifc::manager::ModelManager& manager)
{
	auto loader = manager.GetIfcLoader(modelID);
	switch (t) {
	case webifc::parsing::IfcTokenType::STRING: {
		return loader->GetDecodedStringArgument();
	}
	case webifc::parsing::IfcTokenType::ENUM: {
		std::string_view s = loader->GetStringArgument();
		if (s == "T") return true;
		if (s == "F") return false;
		if (s == "U") return nullptr;
		return std::string(s);
	}
	case webifc::parsing::IfcTokenType::REAL: {
		std::string_view s = loader->GetDoubleArgumentAsString();
		return std::string(s);
	}
	case webifc::parsing::IfcTokenType::INTEGER: {
		long d = loader->GetIntArgument();
		return d;
	}
	case webifc::parsing::IfcTokenType::REF: {
		uint32_t ref = loader->GetRefArgument();
		return ref;
	}
	default:
		return nullptr;
	}
}

static json GetArgs(uint32_t modelID,
	webifc::manager::ModelManager& manager,
	bool inObject = false,
	bool inList = false)
{
	auto loader = manager.GetIfcLoader(modelID);
	json arguments = json::array();
	size_t size = 0;
	bool endOfLine = false;

	while (!loader->IsAtEnd() && !endOfLine) {
		webifc::parsing::IfcTokenType t = loader->GetTokenType();

		switch (t) {
		case webifc::parsing::IfcTokenType::LINE_END: {
			endOfLine = true;
			break;
		}
		case webifc::parsing::IfcTokenType::EMPTY: {
			arguments.push_back(nullptr);
			size++;
			break;
		}
		case webifc::parsing::IfcTokenType::SET_BEGIN: {
			arguments.push_back(GetArgs(modelID, manager, false, true));
			size++;
			break;
		}
		case webifc::parsing::IfcTokenType::SET_END: {
			endOfLine = true;
			break;
		}
		case webifc::parsing::IfcTokenType::LABEL: {
			json obj = json::object();
			obj["type"] = static_cast<uint32_t>(webifc::parsing::IfcTokenType::LABEL);

			loader->StepBack();
			auto s = loader->GetStringArgument();
			auto typeCode = manager.GetSchemaManager().IfcTypeToTypeCode(s);
			obj["typecode"] = typeCode;

			loader->GetTokenType(); // consume set open
			obj["value"] = GetArgs(modelID, manager, true);

			arguments.push_back(obj);
			size++;
			break;
		}
		case webifc::parsing::IfcTokenType::STRING:
		case webifc::parsing::IfcTokenType::ENUM:
		case webifc::parsing::IfcTokenType::REAL:
		case webifc::parsing::IfcTokenType::INTEGER:
		case webifc::parsing::IfcTokenType::REF: {
			loader->StepBack();
			json obj;
			if (inObject) {
				obj = ReadValue(modelID, t, manager);
			}
			else {
				obj = json::object();
				obj["type"] = static_cast<uint32_t>(t);
				obj["value"] = ReadValue(modelID, t, manager);
			}
			arguments.push_back(obj);
			size++;
			break;
		}
		default:
			break;
		}
	}

	if (size == 0 && !inList) return nullptr;
	if (size == 1 && inObject) return arguments[0];
	return arguments;
}

namespace dragon
{
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
		auto args = GetArgs(modelID, manager); 
		std::cout << args.dump(2) << std::endl;
		data["arguments"] = args;
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
	nlohmann::json WebIFCHelper::GetLineFromRawLine(const nlohmann::json& RawLine, webifc::schema::IfcSchemaManager& schemaManager)
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
		}
		else if (type == webifc::schema::IFCRELCONTAINEDINSPATIALSTRUCTURE)
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
						if (name.empty())
						{
							line["name"] = schemaManager.IfcTypeCodeToType(type);
						}
						else
						{
							line["name"] = name;
						}
					}
				}
			}
			else
			{
				line["name"] = schemaManager.IfcTypeCodeToType(type);
			}
		}
		return line;
	}
}