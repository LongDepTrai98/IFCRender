#pragma once 
#ifndef _CESIUM_LAYER_HPP_
#define _CESIUM_LAYER_HPP_
#include <Cesium3DTilesSelection/ViewState.h>
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
	class SimpleAssetAccessor;
}
namespace threepp
{
	class Group; 
}
class CesiumDrawableStyleLayerHost : public mbgl::style::CustomDrawableLayerHost
{
public:
	CesiumDrawableStyleLayerHost(); 
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
	std::unique_ptr<CesiumAsync::AsyncSystem> asyncSystem{ nullptr };
	std::unordered_map<std::string, std::shared_ptr<threepp::Group>> groupResourceCache{};
	std::shared_ptr<Cesium3DTilesSelection::MaplibrePrepareRendererResource> prepareRendererResource{ nullptr }; 
	std::shared_ptr<CesiumNativeTests::SimpleAssetAccessor> mockAssetAccessor{ nullptr };
	std::unique_ptr<CesiumNativeTests::ThreadTaskProcessor> pMockTaskProcessor{ nullptr };
	std::shared_ptr<Cesium3DTilesSelection::TilesetExternals> tilesetExternals{ nullptr }; 
	std::function<void(Interface&,const Cesium3DTilesSelection::BoundingVolume&)> fnc_create_drawable{ nullptr };
}; 
#endif // !_CESIUM_LAYER_HPP_
