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
		std::unordered_map<int, std::vector<int>> chunks{}; 
		getChunks(modelID,m_propsNamesMap["spatial"], chunks);
		getChunks(modelID, m_propsNamesMap["aggregates"], chunks);
		auto tree = createTree(modelID,chunks); 
		/*TEST CHUNK*/
	}
	void IFCProperties::getChunks(const int& modelID, PropsNames prop, std::unordered_map<int, std::vector<int>>& chunk)
	{
		auto loader = m_modelManager->GetIfcLoader(modelID);
		auto ids = loader->GetExpressIDsWithType(prop.expressID);
		for (int i = 0; i < ids.size(); ++i)
		{
			auto RawLine = WebIFCHelper::GetLine(*m_modelManager,modelID,ids[i],true,true);
			auto line = WebIFCHelper::GetLineFromRawLine(RawLine);
			const uint32_t& relatingLineID = line["relating"].get<uint32_t>(); 
			std::vector<uint32_t> vecRelatedLineID;
			vecRelatedLineID.reserve(line["related"].size());
			for (auto& id : line["related"])
			{
				if (id.is_number_integer())
				{
					vecRelatedLineID.emplace_back(id); 
				}
			}
			chunk[relatingLineID].insert(chunk[relatingLineID].end(), vecRelatedLineID.begin(), vecRelatedLineID.end()); 
		}
	}
	std::shared_ptr<IFCProperties::IFCNode> IFCProperties::createTree(const uint32_t& modelID, const std::unordered_map<int, std::vector<int>>& chunk)
	{
		//std::unordered_map<int, std::pair<std::shared_ptr<IFCNode>,bool>> m_mapNodes{};
		std::unordered_map<int, std::shared_ptr<IFCNode>> mapNode{}; 
		std::unordered_map<int, bool> mapCheckParentNode{}; 
		uint32_t parent_ExpressID{ 0 }; 
		//int, shared_ptr<>
		//int, bool isChildren
		for (const auto& [relatingId, relatedIDs] : chunk)
		{
			//is parent node
			std::shared_ptr<IFCNode> parentNode{nullptr}; 
			if (mapNode.find(relatingId) == mapNode.end())
			{
				/*CREATE NEW NODE*/
				parentNode = std::make_shared<IFCNode>(); 
				parentNode->expressID = relatingId;
				mapNode[relatingId] = parentNode;
				mapCheckParentNode[relatingId] = true; 
			}
			else
			{
				parentNode = mapNode[relatingId];
			}
			/*UPDATE STATE PARENT NODE*/
			if (mapCheckParentNode[relatingId])
			{
				mapCheckParentNode[relatingId] = true;
				parent_ExpressID = relatingId;
			}
			for (const auto& relatedID : relatedIDs)
			{
				/*CREATE NEW NODE*/
				std::shared_ptr<IFCNode> childNode{ nullptr }; 
				if (mapNode.find(relatedID) == mapNode.end())
				{
					childNode = std::make_shared<IFCNode>(); 
					childNode->expressID = relatedID; 
					mapNode[relatedID] = childNode;
					mapCheckParentNode[relatedID] = true; 
				}
				else
				{
					childNode = mapNode[relatedID];
				}
				parentNode->children.emplace_back(childNode);
				/*UPDATE STATE CHILD NODE*/
				if (mapCheckParentNode[relatedID])
				{
					mapCheckParentNode[relatedID] = false; 
				}
			}
		}
		auto RawLine = WebIFCHelper::GetLine(*m_modelManager, modelID, parent_ExpressID, true, true);
		spdlog::info(RawLine.dump()); 
		auto parentNode = mapNode[parent_ExpressID];
		return parentNode; 
	}
}