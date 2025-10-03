#include "MaplibreRasterOverlay.hpp"

CesiumAsync::Future<CesiumRasterOverlays::RasterOverlay::CreateTileProviderResult> MaplibreRasterOverlay::createTileProvider(const CesiumAsync::AsyncSystem& asyncSystem,
	const std::shared_ptr<CesiumAsync::IAssetAccessor>& pAssetAccessor,
	const std::shared_ptr<CesiumUtility::CreditSystem>& pCreditSystem,
	const std::shared_ptr<CesiumRasterOverlays::IPrepareRasterOverlayRendererResources>& pPrepareRendererResources,
	const std::shared_ptr<spdlog::logger>& pLogger, CesiumUtility::IntrusivePointer<const RasterOverlay> pOwner) const
{
    const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
    auto projection = CesiumGeospatial::WebMercatorProjection(ellipsoid);
    auto tilingSchemeRectangle = CesiumGeospatial::WebMercatorProjection::MAXIMUM_GLOBE_RECTANGLE;
	CesiumGeometry::Rectangle coverageRectangle = projectRectangleSimple(projection, tilingSchemeRectangle);
    CesiumRasterOverlays::UrlTemplateRasterOverlayOptions options{
     .credit = "Copyright (c) Some Amazing Source",
     .projection = CesiumGeospatial::WebMercatorProjection(),
     .tilingScheme =
         CesiumGeometry::QuadtreeTilingScheme(coverageRectangle, 1, 1),
     .minimumLevel = 0,
     .maximumLevel = 15,
     .tileWidth = 512,
     .tileHeight = 512,
     .coverageRectangle = coverageRectangle,
    };

    CesiumUtility::IntrusivePointer<RasterOverlay> pUrlTemplate =
        new CesiumRasterOverlays::UrlTemplateRasterOverlay(
            this->getName(),
            "https://api.mapbox.com/v4/mapbox.satellite/{z}/{x}/{y}@2x.png?access_token=pk.eyJ1IjoiYW5odHVzeHl6IiwiYSI6ImNsdng4ZGp3ZTA2aDgyaWw3ZnM2NXJhcjcifQ.OV7YSJsVT8zY-L4tozXaVw",
            {},
            options);

    return pUrlTemplate->createTileProvider(
        asyncSystem,
        pAssetAccessor,
        pCreditSystem,
        pPrepareRendererResources,
        pLogger,
        pOwner != nullptr ? pOwner : this);
}
