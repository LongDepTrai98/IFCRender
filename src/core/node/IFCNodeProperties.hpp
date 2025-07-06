#ifndef _NODE_IFC_PROPERTIES_HPP_
#define _NODE_IFC_PROPERTIES_HPP_
#include "INodeProperties.hpp"
#include <vector>
#include <memory>
#include <unordered_map>
namespace dragon
{
	class NodeIFCProperties : public NodeProperties
	{
	public: 
		struct Node {
			int expressID{ -1 }; 
			std::vector<std::shared_ptr<Node>> children{};
		};
	public: 
		NodeIFCProperties() = default; 
		~NodeIFCProperties() = default;
	public: 
		std::shared_ptr<Node> create(const std::unordered_map<int,std::vector<int>>& chunk)
		{
			std::unordered_map<int, std::shared_ptr<NodeIFCProperties::Node>> mapNode{};
			std::unordered_map<int, bool> mapCheckParentNode{};
			uint32_t parent_ExpressID{ 0 };
			//int, shared_ptr<>
			//int, bool isChildren
			for (const auto& [relatingId, relatedIDs] : chunk)
			{
				//is parent node
				std::shared_ptr<NodeIFCProperties::Node> parentNode{ nullptr };
				if (mapNode.find(relatingId) == mapNode.end())
				{
					/*CREATE NEW NODE*/
					parentNode = std::make_shared<NodeIFCProperties::Node>();
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
					std::shared_ptr<NodeIFCProperties::Node> childNode{ nullptr };
					if (mapNode.find(relatedID) == mapNode.end())
					{
						childNode = std::make_shared<NodeIFCProperties::Node>();
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
			return mapNode[parent_ExpressID]; 
		}
	private: 
	};
}
#endif // !_NODE_IFC_PROPERTIES_HPP_
