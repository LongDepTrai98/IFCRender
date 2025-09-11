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

CesiumDrawableStyleLayerHost::CesiumDrawableStyleLayerHost()
{
	auto lambda = [&](Interface& interface) {
		Cesium3DTilesContent::registerAllTileContentTypes();
		mockAssetAccessor = std::make_shared<CesiumNativeTests::MaplibreAssetAccessor>();
		prepareRendererResource = std::make_shared<Cesium3DTilesSelection::MaplibrePrepareRendererResource>();
		tilesetExternals = std::make_shared<Cesium3DTilesSelection::TilesetExternals>(
			mockAssetAccessor,
			prepareRendererResource,
			CesiumAsync::AsyncSystem(std::make_shared<CesiumNativeTests::ThreadTaskProcessor>()),
			nullptr
		);
		auto t = dragon::CesiumHelper::ecefToWgs84(glm::dvec3(-1800081.6336563815,
			6002674.4507558359,
			1182903.2896941833)); 
		std::string path_tileset{ "http://10.222.3.84:9000/3dtiles/root.json"};
		Cesium3DTilesSelection::TilesetOptions options;
		options.maximumScreenSpaceError = 8.0;
		tileset = std::make_shared<Cesium3DTilesSelection::Tileset>(*tilesetExternals,
			path_tileset,
			options);
		isLoadedTileset = true; 
		m_LayerGroup = interface.getLayerGroupBase();
	}; 
	fnc_queue.push(lambda); 

	fnc_create_drawable = [&](Interface& interface, const Cesium3DTilesSelection::BoundingVolume& BoundingVolume)
	{
		glm::dvec3 ecef_center_bounding_volume = dragon::CesiumHelper::getCenterBoundingVolume(BoundingVolume);
		std::optional<glm::dvec3> wgs84_center_bounding_volume = dragon::CesiumHelper::ecefToWgs84(ecef_center_bounding_volume);
		const double lon = wgs84_center_bounding_volume.value().x;
		const double lat = wgs84_center_bounding_volume.value().y;
		auto tile = Convert::wgs84ToTile(lon, lat);
		interface.addCustomDrawableWithTile({ (uint8_t)tile.tileZ, (uint32_t)tile.tileX, (uint32_t)tile.tileY });
		//set scene 
		mbgl::TileLayerGroup* tileLayerGroup = static_cast<mbgl::TileLayerGroup*>(m_LayerGroup.get());
		tileLayerGroup->visitDrawables([&](const mbgl::gfx::Drawable& drawable) {
			if (drawable.getDrawType() == mbgl::gfx::Drawable::DrawableType::DrawableCustom)
			{
				const mbgl::gfx::Drawable* ptrDrawable = &drawable;
				const mbgl::gl::DrawableCustom* ptrDrawableCustom = static_cast<const mbgl::gl::DrawableCustom*>(ptrDrawable);
				if (ptrDrawableCustom)
				{
					auto impl = ptrDrawableCustom->getImpl();
					if (impl->scene)
					{
						auto& tile = ptrDrawable->getTileID();
						scene = impl->scene.get(); 
						prepareRendererResource->context = { impl->scene.get(),&groupResourceCache,tile.value().canonical }; 
						return; 
					}
				}
			}});
	}; 
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
	// Viewport và FOV
	double aspectRatio = state.getSize().aspectRatio();
	glm::dvec2 viewPortSize = glm::dvec2(state.getSize().width, state.getSize().height);
	double horizontalFieldOfView =  state.getFieldOfView();
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
		verticalFieldOfView,
		ellipsoid);
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

	if (isLoadedTileset)
	{
		auto& state = interface.state;
		Cesium3DTilesSelection::ViewState viewstate = createViewState(interface);
		const Cesium3DTilesSelection::ViewUpdateResult& result = tileset->updateView(
			 { viewstate }
		);
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

		auto callback_render_show = [&](const Cesium3DTilesSelection::ViewUpdateResult& result) {
			for (auto& tile : result.tilesToRenderThisFrame)
			{
				if (tile->getState() != Cesium3DTilesSelection::TileLoadState::Done) {
					return;
				}
				const Cesium3DTilesSelection::TileRenderContent* renderContent = tile->getContent().getRenderContent();
				if (renderContent == nullptr) return;
				threepp::Object3D* model_tile = reinterpret_cast<threepp::Object3D*>(renderContent->getRenderResources());
				if (!model_tile) break; 
				model_tile->visible = true;
			}
			//spdlog::info("Tile render this frame : {}", result.tilesToRenderThisFrame.size()); 
		}; 

		auto callback_render_hide = [&](const Cesium3DTilesSelection::ViewUpdateResult& result) {
			for (auto& tile : result.tilesFadingOut)
			{
				if (tile->getState() == Cesium3DTilesSelection::TileLoadState::Failed) {
					std::string tileIdStr = Cesium3DTilesSelection::TileIdUtilities::createTileIdString(tile->getTileID());
					spdlog::error("Failed to load tile : {}", tileIdStr);
					return;
				}
				if (tile->getState() != Cesium3DTilesSelection::TileLoadState::Done) {
					return;
				}
				const Cesium3DTilesSelection::TileContent& content = tile->getContent();
				const Cesium3DTilesSelection::TileRenderContent* renderContent = content.getRenderContent();
				if (renderContent == nullptr) {
					return;
				}
				threepp::Object3D* model_tile = reinterpret_cast<threepp::Object3D*>(renderContent->getRenderResources());
				if (!model_tile) return;
				model_tile->visible = false;
				//spdlog::info("Tile hide this frame : {}", result.tilesFadingOut.size());
			}
		};
		if (scene)
		{
			//scene->traverse([](threepp::Object3D& obj) {
			//	obj.visible = false;
			//	});
			callback_render_show(result);
			callback_render_hide(result);
		}
	}
}

void CesiumDrawableStyleLayerHost::deinitialize()
{
}





