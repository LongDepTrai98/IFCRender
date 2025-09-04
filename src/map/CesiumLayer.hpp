#pragma once 
#ifndef _CESIUM_LAYER_HPP_
#define _CESIUM_LAYER_HPP_
#include <Cesium3DTilesSelection/ViewState.h>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <memory>
#include <functional>
namespace Cesium3DTilesSelection
{
	class Tileset;
	class TilesetExternals; 
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
	std::shared_ptr<Cesium3DTilesSelection::Tileset> tileset{ nullptr };
	std::unique_ptr<CesiumAsync::AsyncSystem> asyncSystem{ nullptr };
	std::shared_ptr<CesiumNativeTests::SimpleAssetAccessor> mockAssetAccessor{ nullptr };
	std::unique_ptr<CesiumNativeTests::ThreadTaskProcessor> pMockTaskProcessor{ nullptr };
	std::shared_ptr<Cesium3DTilesSelection::TilesetExternals> tilesetExternals{ nullptr }; 
	std::function<void(Interface&,const Cesium3DTilesSelection::BoundingVolume&)> fnc_create_drawable{ nullptr };
}; 
#endif // !_CESIUM_LAYER_HPP_
