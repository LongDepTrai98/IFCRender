#include "MaplibrePrepareRendererResource.hpp"

namespace Cesium3DTilesSelection
{
    CesiumAsync::Future<TileLoadResultAndRenderResources> MaplibrePrepareRendererResource::prepareInLoadThread(
            const CesiumAsync::AsyncSystem& asyncSystem,
            TileLoadResult&& tileLoadResult,
            const glm::dmat4& transform,
            const std::any& rendererOptions)
    {
        prepareInLoadThreadTestCallback(tileLoadResult);
        return asyncSystem.createResolvedFuture(TileLoadResultAndRenderResources{
            std::move(tileLoadResult),
            new AllocationResult{totalAllocation} });
    }

    void* MaplibrePrepareRendererResource::prepareInMainThread(
        Cesium3DTilesSelection::Tile& tile,
        void* pLoadThreadResult){
        if (pLoadThreadResult) {
            spdlog::info("Prepare in main thread Tile id : {}", std::get<std::string>(tile.getTileID()));
            AllocationResult* loadThreadResult =
                reinterpret_cast<AllocationResult*>(pLoadThreadResult);
            delete loadThreadResult;
        }

        return new AllocationResult{ totalAllocation };
    }

    void MaplibrePrepareRendererResource::free(
        Cesium3DTilesSelection::Tile& tile,
        void* pLoadThreadResult,
        void* pMainThreadResult) noexcept {
        if (pMainThreadResult) {
            AllocationResult* mainThreadResult =
                reinterpret_cast<AllocationResult*>(pMainThreadResult);
            delete mainThreadResult;
        }

        if (pLoadThreadResult) {
            AllocationResult* loadThreadResult =
                reinterpret_cast<AllocationResult*>(pLoadThreadResult);
            delete loadThreadResult;
        }
    }
}