#pragma once 
#ifndef _MAP_LAYER_ELEMENT_TREE_
#define _MAP_LAYER_ELEMENT_TREE_
#include "ElementTree.hpp"
/*LAYER NODE TREE FOR MAPLIBRE*/
namespace mbgl
{
	namespace style
	{
		class Style; 
	}
}
namespace dragon
{
	class MapLayerTree : public ElementTree
	{
	public:
		struct Node : public TreeNode
		{
		public: 
			std::string getLabelNode() override
			{
				return label;
			}
			int& getID() override
			{
				return id;
			}
		public: 
			std::string label{ "" }; 
			int id{ -1 }; 
		};
	public: 
		std::shared_ptr<Node> create(mbgl::style::Style& style);
	public: 
		std::string label; 
		int id{ -1 }; 
	};
}
#endif // !_MAP_LAYER_ELEMENT_TREE_
