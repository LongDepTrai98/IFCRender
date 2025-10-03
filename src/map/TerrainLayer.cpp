#include "TerrainLayer.hpp"
#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/style/layers/custom_drawable_layer_impl.hpp>
#include <mbgl/renderer/layers/render_custom_drawable_layer.hpp>
#include <mbgl/gfx/drawable.hpp>
#include <mbgl/gl/drawable_custom.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/renderer/buckets/hillshade_bucket.hpp>
#include <mbgl/renderer/buckets/raster_bucket.hpp>
#include <mbgl/renderer/render_tile.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/math/angles.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/util/mat4.hpp>
#include <mbgl/gfx/context.hpp>
#include <mbgl/renderer/layer_group.hpp>
#include <mbgl/gfx/drawable_builder.hpp>
#include <threepp/threepp.hpp>
#include <threepp/textures/Image.hpp>
#include "source/CustomRasterSource.hpp"
#include <fstream>
#include <format>
TerrainStyleLayerHost::TerrainStyleLayerHost(MaplibreCustomRasterSource* source_) : rasterSource(source_)
{
}

TerrainStyleLayerHost::~TerrainStyleLayerHost()
{
}

void TerrainStyleLayerHost::initialize()
{
}

void TerrainStyleLayerHost::deinitialize()
{
}

void TerrainStyleLayerHost::update(Interface& interface)
{
	if (interface.tileToRenderThisFrame->size() == 0) return; 
	for (const mbgl::RenderTile& tile : *interface.tileToRenderThisFrame) 
	{
		const auto& tileID = tile.getOverscaledTileID().canonical;
		if (rasterSource)
		{
			rasterSource->RegisterDownloadTile(tileID); 
		}
	}
	uint8_t current_zoom = static_cast<uint8_t>(interface.state.getZoom()); 
	auto* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(interface.layerGroup.get());
	auto renderPass = mbgl::RenderPass::Translucent;
	tileLayerGroup->visitDrawables([&,renderPass,this, current_zoom](mbgl::gfx::Drawable& drawable) {
		mbgl::gl::DrawableCustom* ptrDrawable_ = static_cast<mbgl::gl::DrawableCustom*>(&drawable);
		auto& uTileID = drawable.getTileID(); 
		threepp::Scene* scene = ptrDrawable_->getImpl()->scene.get(); 
		threepp::Object3D* terrain_mesh = scene->children[0];
		if (this->rasterSource->raster_cache->contains(uTileID.value().canonical))
		{
			if (terrain_mesh)
			{
				threepp::Mesh* mesh = terrain_mesh->as<threepp::Mesh>();
				auto mat = mesh->material()->as<threepp::MeshBasicMaterial>(); 
				if (!mat->map)
				{
					mat->map = rasterSource->raster_cache->getCopy(uTileID.value().canonical);
					mat->map->needsUpdate(); 
					mat->needsUpdate();
					terrain_mesh->visible = true;
				}
			}
		}
	});
}