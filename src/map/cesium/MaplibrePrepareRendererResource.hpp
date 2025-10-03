#pragma once

#include <Cesium3DTilesSelection/IPrepareRendererResources.h>
#include <Cesium3DTilesSelection/Tile.h>
#include <CesiumRasterOverlays/RasterOverlayTile.h>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/renderer/layer_group.hpp>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <spdlog/spdlog.h>
#include <atomic>
#include <functional>
#include <mutex>
#include <unordered_map>

namespace threepp
{
    class Scene; 
    class Group; 
    class Object3D;
    class Texture; 
}

namespace  mbgl::gl
{
    class DrawableCustom; 
}

class MaplibreCustomRasterSource; 
namespace Cesium3DTilesSelection {
class MaplibrePrepareRendererResource : public Cesium3DTilesSelection::IPrepareRendererResources {
public: 
    struct drawable_context
    {
        threepp::Scene* scene{ nullptr };
        mbgl::CanonicalTileID root_tile_id{ 0,0,0 };
        mbgl::LayerGroupBase* layerGroup{ nullptr }; 
        mbgl::style::CustomDrawableLayerHost::Interface* interface{ nullptr };
    };

public:
  std::atomic<size_t> totalAllocation{};

  struct PrepareTileResult {
      PrepareTileResult(std::shared_ptr<threepp::Object3D> obj_, mbgl::CanonicalTileID canonicalTileID_, bool isInScene_)
        : obj(obj_), canonicalTileID(canonicalTileID_), bIsInScene(isInScene_)
      {

      }
      ~PrepareTileResult() noexcept {
          obj = nullptr;
      }
    std::shared_ptr<threepp::Object3D> obj;
    mbgl::CanonicalTileID canonicalTileID{ 0,0,0 };
    bool bIsInScene{ false }; 
    threepp::Scene* scene{ nullptr };
  };

  struct PrepareRasterResult {
      std::shared_ptr<threepp::Texture> texture{ nullptr }; 
  }; 

  ~MaplibrePrepareRendererResource() noexcept {  }

  virtual CesiumAsync::Future<TileLoadResultAndRenderResources> prepareInLoadThread(
          const CesiumAsync::AsyncSystem& asyncSystem,
          TileLoadResult&& tileLoadResult,
          const glm::dmat4& transform,
          const Cesium3DTilesSelection::TileID& tileID,
          const std::any& rendererOptions) override; 

  virtual void* prepareInMainThread(
      Cesium3DTilesSelection::Tile& tile,
      void* pLoadThreadResult) override; 

  virtual void free(
      Cesium3DTilesSelection::Tile& tile,
      void* pLoadThreadResult,
      void* pMainThreadResult) noexcept override; 

  virtual void* prepareRasterInLoadThread(
      CesiumGltf::ImageAsset& image,
      const std::any& rendererOptions) override {
      return new PrepareRasterResult();
  }

  virtual void* prepareRasterInMainThread(
      CesiumRasterOverlays::RasterOverlayTile& rasterTile,
      void* pLoadThreadResult) override {
      int a = 3; 
    if (pLoadThreadResult) {
        PrepareRasterResult* loadThreadResult =
          reinterpret_cast<PrepareRasterResult*>(pLoadThreadResult);
    }
    return pLoadThreadResult; 
  }

  virtual void freeRaster(
      const CesiumRasterOverlays::RasterOverlayTile& /*rasterTile*/,
      void* pLoadThreadResult,
      void* pMainThreadResult) noexcept override {
      if (pMainThreadResult)
      {
          delete pMainThreadResult; 
          pMainThreadResult = nullptr; 
      }
  }

  virtual void attachRasterInMainThread(
      const Cesium3DTilesSelection::Tile& tile,
      int32_t overlayTextureCoordinateID,
      const CesiumRasterOverlays::RasterOverlayTile& rasterTile,
      void* pMainThreadRendererResources,
      const glm::dvec2& translation,
      const glm::dvec2& scale) override {

  }

  virtual void detachRasterInMainThread(
      const Cesium3DTilesSelection::Tile& tile,
      int32_t overlayTextureCoordinateID,
      const CesiumRasterOverlays::RasterOverlayTile& rasterTile,
      void* pMainThreadRendererResources) noexcept override {

  }

  std::function<void(const TileLoadResult&)> prepareInLoadThreadTestCallback =
      [](const TileLoadResult& /*result*/) {};

  std::shared_ptr<threepp::Group> createGroupThreeppFromModel(CesiumGltf::Model& model, 
      glm::dmat4& tile_transform, 
      mbgl::CanonicalTileID& canonicalTileID, 
      std::optional<CesiumGeometry::QuadtreeTileID> geoTileID);
public: 
    std::atomic<int> t_count{ 0 }; 
    std::mutex mutexResource{};
    drawable_context context{};
    std::unordered_map<mbgl::CanonicalTileID, mbgl::gl::DrawableCustom*> setDrawable{}; 
};
} // namespace Cesium3DTilesSelection
