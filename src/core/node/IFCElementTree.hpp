#ifndef _NODE_IFC_PROPERTIES_HPP_
#define _NODE_IFC_PROPERTIES_HPP_
#include "ElementTree.hpp"
#include "core/utils/WebIFCHelper.hpp"
#include "spdlog/spdlog.h"
#include <unordered_map>
namespace dragon
{
	class IFCElementTree : public ElementTree
	{
	public:
		struct Node : public TreeNode
		{
			int expressID{ 0 };
			std::string label{ "" };
		public:
			std::string getLabelNode() override
			{
				return label;
			}
			int& getID() override
			{
				return expressID;
			}
		};
	public:
		IFCElementTree() = default;
		~IFCElementTree() = default;
	public:
		std::shared_ptr<Node> create(const int& modelID, webifc::manager::ModelManager* model_manager, const std::unordered_map<int, std::vector<int>>& chunk)
		{
			std::unordered_map<int, std::shared_ptr<IFCElementTree::Node>> mapNode{};
			std::unordered_map<int, bool> mapCheckParentNode{};
			uint32_t parent_ExpressID{ 0 };
			auto schemaManager = model_manager->GetSchemaManager();
			for (const auto& [relatingId, relatedIDs] : chunk)
			{
				//is parent node
				std::shared_ptr<IFCElementTree::Node> parentNode{ nullptr };
				if (mapNode.find(relatingId) == mapNode.end())
				{
					/*CREATE NEW NODE*/
					auto RawLine = WebIFCHelper::GetLine(*model_manager, modelID, relatingId, true, true);
					if (RawLine.is_null())
					{
						continue;
					}
					parentNode = std::make_shared<IFCElementTree::Node>();
					parentNode->expressID = relatingId;
					parentNode->label = WebIFCHelper::GetLineFromRawLine(RawLine, schemaManager)["name"];
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
					std::shared_ptr<IFCElementTree::Node> childNode{ nullptr };
					if (mapNode.find(relatedID) == mapNode.end())
					{
						auto RawLine = WebIFCHelper::GetLine(*model_manager, modelID, relatedID, true, true);
						if (RawLine.is_null())
						{
							continue;
						}
						childNode = std::make_shared<IFCElementTree::Node>();
						childNode->expressID = relatedID;
						childNode->label = WebIFCHelper::GetLineFromRawLine(RawLine, schemaManager)["name"];
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
			m_Parent = mapNode[parent_ExpressID];
			return mapNode[parent_ExpressID];
		}
	public:
		std::shared_ptr<Node> parent_node{ nullptr };
	};
}
#endif // !_NODE_IFC_PROPERTIES_HPP_