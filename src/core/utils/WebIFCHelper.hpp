#ifndef _WEB_IFC_HELPER_HPP_
#define _WEB_IFC_HELPER_HPP_
#include "web-ifc/parsing/IfcLoader.h"
#include "web-ifc/schema/IfcSchemaManager.h"
#include "web-ifc/schema/ifc-schema.h"
#include "web-ifc/modelmanager/ModelManager.h"
#include "web-ifc/geometry/IfcGeometryProcessor.h"
#include <nlohmann/json.hpp>
#include <variant>
#include <string_view>
#include <optional>
namespace dragon
{
	class WebIFCHelper
	{
	public: 
		using variant_value = std::variant<std::string,
			bool,
			std::string_view,
			int,
			float,
			double,
			uint32_t>; 
	public:
		static nlohmann::json ReadValue(webifc::manager::ModelManager& manager, uint32_t modelID, webifc::parsing::IfcTokenType t); 
		static nlohmann::json GetArgs(webifc::manager::ModelManager& manager, uint32_t modelID, bool inObject = false, bool inList = false);
	private:
		WebIFCHelper() = default;
	}; 
}
#endif // !_WEB_IFC_HELPER_HPP_
