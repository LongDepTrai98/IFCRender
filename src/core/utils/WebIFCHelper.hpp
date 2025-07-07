#ifndef _WEB_IFC_HELPER_HPP_
#define _WEB_IFC_HELPER_HPP_
#include "web-ifc/parsing/IfcLoader.h"
#include "web-ifc/schema/IfcSchemaManager.h"
#include "web-ifc/schema/ifc-schema.h"
#include "web-ifc/modelmanager/ModelManager.h"
#include "web-ifc/geometry/IfcGeometryProcessor.h"
#include <nlohmann/json.hpp>
#include <optional>
#include <vector>
namespace dragon
{
	class WebIFCHelper
	{
	public:
		static nlohmann::json ReadValue(webifc::manager::ModelManager& manager, uint32_t modelID, webifc::parsing::IfcTokenType t);
		static nlohmann::json GetArgs(webifc::manager::ModelManager& manager, uint32_t modelID, bool inObject = false, bool inList = false);
		static nlohmann::json GetLine(webifc::manager::ModelManager& manager, const uint32_t& modelID, const uint32_t& line, bool flatten, bool inverse);
		static std::vector<uint32_t> GetLineIDsWithType(webifc::manager::ModelManager& manager, const uint32_t& modelID, const uint32_t& expressID);
		static nlohmann::json GetLineFromRawLine(const nlohmann::json& json, webifc::schema::IfcSchemaManager& schemaManager);
	private:
		WebIFCHelper() = default;
	};
}
#endif // !_WEB_IFC_HELPER_HPP_