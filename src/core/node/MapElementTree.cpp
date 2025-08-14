#include <mbgl/gfx/backend.hpp>
#include <mbgl/renderer/renderer.hpp>
#include <mbgl/storage/database_file_source.hpp>
#include <mbgl/storage/file_source_manager.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/util/platform.hpp>
#include <mbgl/util/string.hpp>
#include <mbgl/util/mapbox.hpp>
#include <mbgl/style/layers/fill_layer.hpp>
#include <mbgl/util/instrumentation.hpp>
#include <mbgl/style/expression/dsl.hpp>
#include <mbgl/style/types.hpp>
#include <mbgl/style/layers/fill_extrusion_layer.hpp>
#include "MapElementTree.hpp"
namespace dragon
{
	std::shared_ptr<MapLayerTree::Node> MapLayerTree::create(mbgl::style::Style& style)
	{
		std::shared_ptr<Node> root_node = std::make_shared<Node>(); 
		root_node->label = style.getName();
		for (const auto& layer : style.getLayers())
		{
			std::shared_ptr<Node> child_node = std::make_shared<Node>(); 
			child_node->label = layer->getID(); 
			root_node->children.emplace_back(child_node); 
		}
		m_Parent = root_node; 
		return root_node;
	}
}