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
        std::shared_ptr<Node> createSpatialAndGroupByType(
            const int& modelID,
            webifc::manager::ModelManager* model_manager,
            const std::unordered_map<int, std::vector<int>>& spatial_chunks,
            const std::unordered_map<int, std::vector<int>>& aggregates_chunks)
        {
            std::unordered_map<int, std::shared_ptr<IFCElementTree::Node>> mapNode{};
            std::map<std::pair<int, std::string>, std::shared_ptr<IFCElementTree::Node>> bucketMap{};
            auto schemaManager = model_manager->GetSchemaManager();

            auto getOrCreateNode = [&](int expressID) -> std::shared_ptr<IFCElementTree::Node> {
                if (mapNode.find(expressID) != mapNode.end()) {
                    return mapNode[expressID];
                }
                auto RawLine = WebIFCHelper::GetLine(*model_manager, modelID, expressID, true, true);
                if (RawLine.is_null()) return nullptr;

                auto node = std::make_shared<IFCElementTree::Node>();
                node->expressID = expressID;
                node->label = WebIFCHelper::GetLineFromRawLine(RawLine, schemaManager)["name"];
                mapNode[expressID] = node;
                return node;
                };

            auto attachChildren = [&](int parentId, const std::vector<int>& children) {
                auto parentNode = getOrCreateNode(parentId);
                if (!parentNode) return;

                // lấy type của parent
                auto parentRaw = WebIFCHelper::GetLine(*model_manager, modelID, parentId, true, true);
                int parentTypeCode = parentRaw["type"];
                std::string parentTypeName = schemaManager.IfcTypeCodeToType(parentTypeCode);

                for (auto childId : children) {
                    auto childNode = getOrCreateNode(childId);
                    if (!childNode) continue;

                    auto childRaw = WebIFCHelper::GetLine(*model_manager, modelID, childId, true, true);
                    int childTypeCode = childRaw["type"];
                    std::string childTypeName = schemaManager.IfcTypeCodeToType(childTypeCode);

                    if (childTypeName == parentTypeName) {
                        // cùng loại => gắn trực tiếp
                        parentNode->children.emplace_back(childNode);
                        continue;
                    }

                    // khác loại => group theo type
                    auto key = std::make_pair(parentId, childTypeName);

                    std::shared_ptr<IFCElementTree::Node> bucketNode;
                    if (bucketMap.find(key) == bucketMap.end()) {
                        bucketNode = std::make_shared<IFCElementTree::Node>();
                        bucketNode->expressID = -1;
                        bucketNode->label = childTypeName;
                        parentNode->children.emplace_back(bucketNode);
                        bucketMap[key] = bucketNode;
                    }
                    else {
                        bucketNode = bucketMap[key];
                    }

                    bucketNode->children.emplace_back(childNode);
                }
                };

            // xử lý spatial relationships
            for (const auto& [parentId, children] : spatial_chunks) {
                attachChildren(parentId, children);
            }

            // xử lý aggregates relationships
            for (const auto& [parentId, children] : aggregates_chunks) {
                attachChildren(parentId, children);
            }

            // lấy root (thường là IfcProject)
            int rootId = aggregates_chunks.begin()->first;
            m_Parent = mapNode[rootId]; 
            return mapNode[rootId];
        }
	public:
		std::shared_ptr<Node> parent_node{ nullptr };
	};
}
#endif // !_NODE_IFC_PROPERTIES_HPP_