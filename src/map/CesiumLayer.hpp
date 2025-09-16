#pragma once 
#ifndef _CESIUM_LAYER_HPP_
#define _CESIUM_LAYER_HPP_
#include <Cesium3DTilesSelection/ViewState.h>
#include <CesiumAsync/AsyncSystem.h>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <memory>
#include <functional>
#include <queue>
#include <atomic>
#include <map>
namespace Cesium3DTilesSelection
{
	class Tileset;
	class TilesetExternals; 
	class MaplibrePrepareRendererResource; 
}
namespace CesiumNativeTests
{
	class ThreadTaskProcessor;
}
namespace CesiumAsync
{
	class AsyncSystem; 
}
namespace CesiumNativeTests
{
	class MaplibreAssetAccessor;
}
namespace threepp
{
	class Group; 
}
class CesiumDrawableStyleLayerHost : public mbgl::style::CustomDrawableLayerHost
{
public:
	CesiumDrawableStyleLayerHost(std::string path_tileset);
	~CesiumDrawableStyleLayerHost(); 
public: 
	void initialize() override;
	void deinitialize() override;
	void update(Interface& interface) override;
private: 
	Cesium3DTilesSelection::ViewState createViewState(Interface& interface); 
public: 
	std::atomic<bool> isLoadedTileset{ false }; 
	std::queue<std::function<void(Interface&)>> fnc_queue{};
	std::shared_ptr<mbgl::LayerGroupBase> m_LayerGroup{ nullptr };
	std::shared_ptr<Cesium3DTilesSelection::Tileset> tileset{ nullptr };
	threepp::Scene* scene{ nullptr };
	std::shared_ptr<Cesium3DTilesSelection::MaplibrePrepareRendererResource> prepareRendererResource{ nullptr }; 
	std::shared_ptr<CesiumNativeTests::MaplibreAssetAccessor> mockAssetAccessor{ nullptr };
	std::shared_ptr<Cesium3DTilesSelection::TilesetExternals> tilesetExternals{ nullptr }; 
	std::function<void(Interface&,const Cesium3DTilesSelection::BoundingVolume&)> fnc_create_drawable{ nullptr };
}; 
#endif // !_CESIUM_LAYER_HPP_
