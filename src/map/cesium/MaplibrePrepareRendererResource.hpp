#pragma once

#include <Cesium3DTilesSelection/IPrepareRendererResources.h>
#include <Cesium3DTilesSelection/Tile.h>
#include <CesiumRasterOverlays/RasterOverlayTile.h>
#include <spdlog/spdlog.h>
#include <atomic>
#include <functional>
#include <mbgl/tile/tile_id.hpp>
#include <mutex>


namespace threepp
{
    class Scene; 
    class Group; 
    class Object3D; 
}

namespace Cesium3DTilesSelection {
class MaplibrePrepareRendererResource : public Cesium3DTilesSelection::IPrepareRendererResources {
public: 
    struct drawable_context
    {
        threepp::Scene* scene{ nullptr };
        mbgl::CanonicalTileID root_tile_id{ 0,0,0 };
    };

public:
  std::atomic<size_t> totalAllocation{};

  struct PrepareResult {
      PrepareResult(std::shared_ptr<threepp::Object3D> obj_)
        : obj{ obj_ } {
    }
      ~PrepareResult() noexcept { obj = nullptr; }
    std::shared_ptr<threepp::Object3D> obj;
  };

  ~MaplibrePrepareRendererResource() noexcept {  }

  virtual CesiumAsync::Future<TileLoadResultAndRenderResources>
      prepareInLoadThread(
          const CesiumAsync::AsyncSystem& asyncSystem,
          TileLoadResult&& tileLoadResult,
          const glm::dmat4& transform,
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
    return new PrepareResult{nullptr};
  }

  virtual void* prepareRasterInMainThread(
      CesiumRasterOverlays::RasterOverlayTile& /*rasterTile*/,
      void* pLoadThreadResult) override {
    if (pLoadThreadResult) {
        PrepareResult* loadThreadResult =
          reinterpret_cast<PrepareResult*>(pLoadThreadResult);
      delete loadThreadResult;
    }

    return new PrepareResult{nullptr};
  }

  virtual void freeRaster(
      const CesiumRasterOverlays::RasterOverlayTile& /*rasterTile*/,
      void* pLoadThreadResult,
      void* pMainThreadResult) noexcept override {
    if (pMainThreadResult) {
        PrepareResult* mainThreadResult =
          reinterpret_cast<PrepareResult*>(pMainThreadResult);
      delete mainThreadResult;
    }

    if (pLoadThreadResult) {
        PrepareResult* loadThreadResult =
          reinterpret_cast<PrepareResult*>(pLoadThreadResult);
      delete loadThreadResult;
    }
  }

  virtual void attachRasterInMainThread(
      const Cesium3DTilesSelection::Tile& tile,
      int32_t overlayTextureCoordinateID,
      const CesiumRasterOverlays::RasterOverlayTile& rasterTile,
      void* pMainThreadRendererResources,
      const glm::dvec2& translation,
      const glm::dvec2& scale) override {}

  virtual void detachRasterInMainThread(
      const Cesium3DTilesSelection::Tile& tile,
      int32_t overlayTextureCoordinateID,
      const CesiumRasterOverlays::RasterOverlayTile& rasterTile,
      void* pMainThreadRendererResources) noexcept override {}

  std::function<void(const TileLoadResult&)> prepareInLoadThreadTestCallback =
      [](const TileLoadResult& /*result*/) {};

  std::shared_ptr<threepp::Group> createGroupThreeppFromModel(Cesium3DTilesSelection::Tile& tile);
  std::shared_ptr<threepp::Group> createGroupThreeppFromModel(CesiumGltf::Model& model, 
      glm::dmat4& tile_transform);
public: 
    std::atomic<int> t_count{ 0 }; 
    std::mutex mutexResource{};
    drawable_context context{};
};
} // namespace Cesium3DTilesSelection
