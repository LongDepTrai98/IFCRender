#include <Cesium3DTiles/GroupMetadata.h>
#include <Cesium3DTiles/MetadataQuery.h>
#include <Cesium3DTiles/Schema.h>
#include <Cesium3DTilesContent/registerAllTileContentTypes.h>
#include <Cesium3DTilesSelection/Tile.h>
#include <Cesium3DTilesSelection/TileContent.h>
#include <Cesium3DTilesSelection/TileLoadResult.h>
#include <Cesium3DTilesSelection/Tileset.h>
#include <Cesium3DTilesSelection/TilesetContentLoader.h>
#include <Cesium3DTilesSelection/TilesetExternals.h>
#include <Cesium3DTilesSelection/ViewState.h>
#include <Cesium3DTilesSelection/ViewUpdateResult.h>
#include <CesiumAsync/AsyncSystem.h>
#include <CesiumAsync/Future.h>
#include <CesiumAsync/Promise.h>
#include <CesiumGeospatial/BoundingRegion.h>
#include <CesiumGeospatial/Cartographic.h>
#include <CesiumGeospatial/Ellipsoid.h>
#include <CesiumGeospatial/GlobeRectangle.h>
#include <CesiumGeospatial/S2CellBoundingVolume.h>
#include <CesiumGeospatial/GlobeTransforms.h>
#include <CesiumGeometry/Transforms.h>
#include <CesiumUtility/Math.h>
#include "CesiumLayer.hpp"
#include "cesium/ThreadTaskProcessor.hpp"
#include "cesium/SimplePrepareRendererResource.hpp"
#include "cesium/SimpleTaskProcessor.hpp"
#include "cesium/SimpleAssetAccessor.hpp"

CesiumDrawableStyleLayerHost::CesiumDrawableStyleLayerHost()
{
	Cesium3DTilesContent::registerAllTileContentTypes();
	CesiumGeospatial::Cartographic position = CesiumGeospatial::Cartographic::fromDegrees(106.69425959750991, 10.770555368200014, 0.0);
	glm::dvec3 ecef = CesiumGeospatial::Ellipsoid::WGS84.cartographicToCartesian(position);
	glm::dmat4 enuMatrix = CesiumGeospatial::GlobeTransforms::eastNorthUpToFixedFrame(ecef);

	mockAssetAccessor = std::make_shared<CesiumNativeTests::SimpleAssetAccessor>(); 
	tilesetExternals = std::make_shared<Cesium3DTilesSelection::TilesetExternals>(
		mockAssetAccessor,
		std::make_shared<Cesium3DTilesSelection::SimplePrepareRendererResource>(),
		CesiumAsync::AsyncSystem(std::make_shared<CesiumNativeTests::SimpleTaskProcessor>()),
		nullptr
	); 

	std::string path_tileset{ "D:\\Code\\3d-tiles-samples\\1.1\\MetadataGranularities\\tileset.json" }; 
	//Cesium3DTilesSelection::TilesetOptions options{};
	tileset = std::make_shared<Cesium3DTilesSelection::Tileset>(*tilesetExternals,
		path_tileset); 
	tileset->loadTiles();
	Cesium3DTilesSelection::Tile* pTilesetJson = const_cast<Cesium3DTilesSelection::Tile*>(tileset->getRootTile());
	pTilesetJson->setTransform(glm::dmat4());
	int a = 3; 
}

CesiumDrawableStyleLayerHost::~CesiumDrawableStyleLayerHost()
{
}

void CesiumDrawableStyleLayerHost::initialize()
{
}

void CesiumDrawableStyleLayerHost::update(Interface& interface)
{
	const mbgl::TransformState& state = interface.state; 
	const Cesium3DTilesSelection::Tile* pTilesetJson = tileset->getRootTile();
	tileset->loadedTiles(); 
	if (pTilesetJson)
	{
		int a = 3; 
	}
}

void CesiumDrawableStyleLayerHost::deinitialize()
{
}
