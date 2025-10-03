#ifndef _MAPLIBRE_RASTER_OVERLAY_HPP_
#define _MAPLIBRE_RASTER_OVERLAY_HPP_
#include <CesiumRasterOverlays/RasterOverlay.h>
#include <CesiumRasterOverlays/UrlTemplateRasterOverlay.h>
#include <Cesium3DTilesSelection/IPrepareRendererResources.h>
#include <string>
class MaplibreRasterOverlay : public CesiumRasterOverlays::RasterOverlay
{
public:
	MaplibreRasterOverlay(std::string name, 
		const CesiumRasterOverlays::RasterOverlayOptions& options) : RasterOverlay(name, options){}
public: 
    virtual CesiumAsync::Future<CreateTileProviderResult> createTileProvider(
        const CesiumAsync::AsyncSystem& asyncSystem,
        const std::shared_ptr<CesiumAsync::IAssetAccessor>& pAssetAccessor,
        const std::shared_ptr<CesiumUtility::CreditSystem>& pCreditSystem,
        const std::shared_ptr<CesiumRasterOverlays::IPrepareRasterOverlayRendererResources>& pPrepareRendererResources,
        const std::shared_ptr<spdlog::logger>& pLogger,
        CesiumUtility::IntrusivePointer<const RasterOverlay> pOwner)
        const override;
};
#endif // !_MAPLIBRE_RASTER_OVERLAY_HPP_
