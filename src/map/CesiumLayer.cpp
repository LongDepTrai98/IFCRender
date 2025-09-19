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
#include <CesiumIonClient/ApplicationData.h>
#include <CesiumIonClient/Connection.h>
#include <CesiumIonClient/Defaults.h>
#include <CesiumIonClient/Geocoder.h>
#include <CesiumIonClient/Profile.h>
#include <CesiumIonClient/Response.h>
#include <CesiumUtility/Math.h>
#include <mbgl/style/layer.hpp>
#include <mbgl/style/layers/custom_drawable_layer.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/gfx/drawable.hpp>
#include <mbgl/gl/drawable_custom.hpp>
#include <mbgl/gl/drawable_custom_impl.hpp>
#include <mbgl/util/logging.hpp>
#include <mbgl/renderer/layer_group.hpp>
#include <format>
#include <glm/gtc/type_ptr.hpp>
#include "CesiumLayer.hpp"
#include "cesium/ThreadTaskProcessor.hpp"
#include "cesium/MaplibrePrepareRendererResource.hpp"
#include "cesium/SimpleTaskProcessor.hpp"
#include "cesium/MaplibreAssetAccessor.hpp"
#include "threepp/threepp.hpp"
#include "TilesetJsonLoader.h"
#include "core/utils/CesiumHelper.hpp"
#include "core/convert/Tile.hpp"
#include <CesiumAsync/CachingAssetAccessor.h>
#include <CesiumAsync/GunzipAssetAccessor.h>
CesiumDrawableStyleLayerHost::CesiumDrawableStyleLayerHost(std::string path_tileset)
{
	auto lambda = [&, path_tileset](Interface& interface) {
		mockAssetAccessor = std::make_shared<CesiumNativeTests::MaplibreAssetAccessor>();
		prepareRendererResource = std::make_shared<Cesium3DTilesSelection::MaplibrePrepareRendererResource>();
		gunzipAssetAccessor = std::make_shared<CesiumAsync::GunzipAssetAccessor>(
			mockAssetAccessor
		);
		tilesetExternals = std::make_shared<Cesium3DTilesSelection::TilesetExternals>(
			gunzipAssetAccessor,
			prepareRendererResource,
			CesiumAsync::AsyncSystem(std::make_shared<CesiumNativeTests::ThreadTaskProcessor>()),
			nullptr
		);
		Cesium3DTilesSelection::TilesetOptions options;
		options.mainThreadLoadingTimeLimit = 5.0; 
		options.mainThreadLoadingTimeLimit = 5.0; 
		options.contentOptions.applyTextureTransform = false; 
		CesiumGltf::SupportedGpuCompressedPixelFormats supportedFormats;
		supportedFormats.ETC1_RGB = true;
		supportedFormats.BC1_RGB = true;
		supportedFormats.BC3_RGBA = true;
		supportedFormats.ASTC_4x4_RGBA = true;
		supportedFormats.ETC2_EAC_R11 = true;
		supportedFormats.ETC2_EAC_RG11 = true;
		supportedFormats.BC7_RGBA = true;
		options.contentOptions.ktx2TranscodeTargets = { supportedFormats, true };
		options.loadErrorCallback = [=](const Cesium3DTilesSelection::TilesetLoadFailureDetails& failData) {
			spdlog::error("Failed to load a given tileset, error: {} ", failData.message.c_str());
		};
		Cesium3DTilesContent::registerAllTileContentTypes();
		if (!path_tileset.empty())
		{
			tileset = std::make_shared<Cesium3DTilesSelection::Tileset>(*tilesetExternals,
				path_tileset,
				options); 
		}
		else
		{
			tileset = std::make_shared<Cesium3DTilesSelection::Tileset>(*tilesetExternals,
				2275207,
				"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJqdGkiOiJmZDAyNWYzMi1mMjk4LTQ5NjEtYmUwMi1hNjc4MDcxOWFlNDQiLCJpZCI6MTc4MTMzLCJpYXQiOjE2OTk5NDU5Njd9.7K9lFmBsKk4H1tfTfn590iTEGcFGqOXsa25XO8LoXJ4",
				options);
		}
		isLoadedTileset = true; 
		m_LayerGroup = interface.getLayerGroupBase();
		prepareRendererResource->context.layerGroup = m_LayerGroup.get(); 
	}; 
	fnc_queue.push(lambda); 
}

CesiumDrawableStyleLayerHost::~CesiumDrawableStyleLayerHost()
{
	tileset.reset(); 
	tilesetExternals.reset(); 
	prepareRendererResource.reset(); 
	mockAssetAccessor.reset(); 
}

void CesiumDrawableStyleLayerHost::initialize()
{
}

Cesium3DTilesSelection::ViewState CesiumDrawableStyleLayerHost::createViewState(Interface& interface)
{
	const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
	const mbgl::TransformState& state = interface.state; 
	const mbgl::LatLng& center = state.getLatLng();
	double center_altitude = 0.0;
	CesiumGeospatial::Cartographic radian_center_location = CesiumGeospatial::Cartographic::fromDegrees(center.longitude(), center.latitude(), center_altitude);
	glm::dvec3 ecef_center = dragon::CesiumHelper::wgs84ToEcef(center.longitude(),center.latitude(),center_altitude);
	const mbgl::FreeCameraOptions& free_cam_options = state.getFreeCameraOptions();
	const mbgl::LatLng& location_cam_wgs84 = free_cam_options.getLocation().value().location;//lat lon 
	const double& cam_altitude_wgs84 = free_cam_options.getLocation().value().altitude; //
	CesiumGeospatial::Cartographic radian_camera_location = CesiumGeospatial::Cartographic::fromDegrees(location_cam_wgs84.longitude(), location_cam_wgs84.latitude(), cam_altitude_wgs84);
	glm::dvec3 ecef_camera_location = dragon::CesiumHelper::wgs84ToEcef(location_cam_wgs84.longitude(), location_cam_wgs84.latitude(), cam_altitude_wgs84);
	glm::dvec3 worldUp = ellipsoid.geodeticSurfaceNormal(ecef_camera_location);
	double aspectRatio = state.getSize().aspectRatio();
	glm::dvec2 viewPortSize = glm::dvec2(state.getSize().width, state.getSize().height);
	double degMaplibreCam = CesiumUtility::Math::radiansToDegrees(state.getFieldOfView()); 
	double horizontalFieldOfView = CesiumUtility::Math::degreesToRadians(degMaplibreCam * 2.0);
	double verticalFieldOfView = std::atan(std::tan(horizontalFieldOfView * 0.5) / aspectRatio) * 2.0;
	glm::dvec3 direction = glm::normalize(ecef_center - ecef_camera_location);
	glm::dvec3 right = glm::normalize(glm::cross(direction, worldUp)); 
	glm::dvec3 up = glm::cross(right, direction); 
	return Cesium3DTilesSelection::ViewState(
		ecef_camera_location,
		direction,
		up,
		viewPortSize,
		horizontalFieldOfView,
		verticalFieldOfView * 1.5);
}

void CesiumDrawableStyleLayerHost::update(Interface& interface)
{
	if (!fnc_queue.empty())
	{
		std::function<void(Interface&)> fnc = fnc_queue.front(); 
		if (fnc)
		{
			fnc(interface); 
			fnc_queue.pop(); 
		}
	}

	prepareRendererResource->context.interface = &interface;
	auto& state = interface.state;
	Cesium3DTilesSelection::ViewState viewstate = createViewState(interface);
	const Cesium3DTilesSelection::ViewUpdateResult& result = tileset->updateView({ viewstate },0.0f);


	if (fnc_create_drawable)
	{
		if (auto tile_root = tileset->getRootTile())
		{
			//hard code get center
			const Cesium3DTilesSelection::BoundingVolume& BoundingVolume = tile_root->getBoundingVolume();
			fnc_create_drawable(interface, BoundingVolume);
			fnc_create_drawable = nullptr;
		}
	}
	{
		auto callback_render_show = [&](const Cesium3DTilesSelection::ViewUpdateResult& result) {
			for (auto& tile : result.tilesToRenderThisFrame)
			{
				if (tile->getState() != Cesium3DTilesSelection::TileLoadState::Done) {
					continue;
				}
				const Cesium3DTilesSelection::TileRenderContent* renderContent = tile->getContent().getRenderContent();
				if (renderContent == nullptr)
				{
					spdlog::error("error"); 
					continue;
				}
				Cesium3DTilesSelection::MaplibrePrepareRendererResource::PrepareTileResult* ptr_model = reinterpret_cast<Cesium3DTilesSelection::MaplibrePrepareRendererResource::PrepareTileResult*>(renderContent->getRenderResources());
				if (!ptr_model) continue;
				ptr_model->obj->visible = true;
				ptr_model->bIsInScene = true;
			}
			};

		auto callback_render_hide = [&](const Cesium3DTilesSelection::ViewUpdateResult& result) {
			for (auto& tile : result.tilesFadingOut)
			{
				if (tile->getState() == Cesium3DTilesSelection::TileLoadState::Failed) {
					std::string tileIdStr = Cesium3DTilesSelection::TileIdUtilities::createTileIdString(tile->getTileID());
					continue;
				}
				if (tile->getState() != Cesium3DTilesSelection::TileLoadState::Done) {
					continue;
				}
				const Cesium3DTilesSelection::TileContent& content = tile->getContent();
				const Cesium3DTilesSelection::TileRenderContent* renderContent = content.getRenderContent();
				if (renderContent == nullptr) {
					continue;
				}
				Cesium3DTilesSelection::MaplibrePrepareRendererResource::PrepareTileResult* ptr_model = reinterpret_cast<Cesium3DTilesSelection::MaplibrePrepareRendererResource::PrepareTileResult*>(renderContent->getRenderResources());
				if (!ptr_model) continue;
				if (!ptr_model->bIsInScene) continue; 
				ptr_model->obj->visible = false;
			}
			};
			callback_render_show(result);
			callback_render_hide(result);
	}
}

void CesiumDrawableStyleLayerHost::deinitialize()
{
}





