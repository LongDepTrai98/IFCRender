#ifndef _TERRAIN_LAYER_HPP_
#define _TERRAIN_LAYER_HPP_
#include <mbgl/style/layers/custom_drawable_layer.hpp>

class MaplibreCustomRasterSource; 
class TerrainStyleLayerHost : public mbgl::style::CustomDrawableLayerHost
{
public: 
	TerrainStyleLayerHost(MaplibreCustomRasterSource* source = nullptr);
	~TerrainStyleLayerHost(); 
public: 
	void initialize() override;
	void deinitialize() override;
	void update(Interface& interface) override;
	MaplibreCustomRasterSource* rasterSource; 
};
#endif // !_TERRAIN_LAYER_HPP_
