#include "IFCProperties.hpp"
#include "core/utils/WebIFCHelper.hpp"
#include "spdlog/spdlog.h"
namespace dragon
{
	IFCProperties::IFCProperties(const int& modelID, 
		webifc::manager::ModelManager* model) : m_modelManager(model)
	{
		m_propsNamesMap = {
			{ "aggregates", { webifc::schema::IFCRELAGGREGATES, "RelatingObject", "RelatedObjects", "children" } },
			{ "spatial",    { webifc::schema::IFCRELCONTAINEDINSPATIALSTRUCTURE, "RelatingStructure", "RelatedElements", "children" } },
			{ "psets",      { webifc::schema::IFCRELDEFINESBYPROPERTIES, "RelatingPropertyDefinition", "RelatedObjects", "IsDefinedBy" } },
			{ "materials",  { webifc::schema::IFCRELASSOCIATESMATERIAL, "RelatingMaterial", "RelatedObjects", "HasAssociations" } },
			{ "type",       { webifc::schema::IFCRELDEFINESBYTYPE, "RelatingType", "RelatedObjects", "IsDefinedBy" } }
		};
		getSpatialTreeChunks(modelID); 
	}
	IFCProperties::~IFCProperties()
	{
		m_modelManager = nullptr;
	}
	void IFCProperties::getProperties()
	{

	}
	void IFCProperties::getSpatialTreeChunks(const int& modelID)
	{
		getChunks(modelID, m_propsNamesMap["aggregates"]);
	}
	void IFCProperties::getChunks(const int& modelID, PropsNames prop)
	{
		auto loader = m_modelManager->GetIfcLoader(modelID);
		auto ids = loader->GetExpressIDsWithType(prop.expressID);
		for (int i = 0; i < ids.size(); ++i)
		{
			auto line = WebIFCHelper::getLine(*m_modelManager,modelID,ids[i],true,true);
			spdlog::info(line.dump()); 
		}
	}
}