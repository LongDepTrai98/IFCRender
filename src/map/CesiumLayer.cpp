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
#include <format>
#include "CesiumLayer.hpp"
#include "cesium/ThreadTaskProcessor.hpp"
#include "cesium/SimplePrepareRendererResource.hpp"
#include "cesium/SimpleTaskProcessor.hpp"
#include "cesium/SimpleAssetAccessor.hpp"
#include "threepp/threepp.hpp"
#include "TilesetJsonLoader.h"
#include <glm/gtc/type_ptr.hpp>

static void saveMatrixToFile(const glm::dmat4& m, const std::string& filename) {
	std::ofstream out(filename);
	if (!out.is_open()) {
		std::cerr << "Không mở được file: " << filename << std::endl;
		return;
	}

	const double* ptr = glm::value_ptr(m); // Lấy con trỏ tới data
	for (int i = 0; i < 16; i++) {
		out << ptr[i];
		if (i < 15) out << " ";
	}
	out << std::endl;

	out.close();
	std::cout << "Đã ghi ma trận vào file " << filename << std::endl;
}

static void printMatrix(const glm::dmat4& M) {
	const double* p = glm::value_ptr(M); // lấy pointer tới 16 phần tử (column-major)
	std::cout << std::fixed << std::setprecision(12);
	for (int i = 0; i < 16; ++i) {
		std::cout << p[i];
		if (i != 15) std::cout << ",";
	}
	std::cout << std::endl;
}

CesiumDrawableStyleLayerHost::CesiumDrawableStyleLayerHost()
{
	Cesium3DTilesContent::registerAllTileContentTypes();
	const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
	CesiumGeospatial::Cartographic position = CesiumGeospatial::Cartographic::fromDegrees(-75.152325, 39.94704, 0.0);
	glm::dvec3 ecef = ellipsoid.cartographicToCartesian(position);
	glm::dmat4 enuMatrix = CesiumGeospatial::GlobeTransforms::eastNorthUpToFixedFrame(ecef);
	printMatrix(enuMatrix);
	mockAssetAccessor = std::make_shared<CesiumNativeTests::SimpleAssetAccessor>(); 
	tilesetExternals = std::make_shared<Cesium3DTilesSelection::TilesetExternals>(
		mockAssetAccessor,
		std::make_shared<Cesium3DTilesSelection::SimplePrepareRendererResource>(),
		CesiumAsync::AsyncSystem(std::make_shared<CesiumNativeTests::SimpleTaskProcessor>()),
		nullptr
	); 
	std::string path_tileset{"D:\\Code\\3d-tiles-samples\\1.1\\MetadataGranularities\\tileset.json" }; 
	//Cesium3DTilesSelection::TilesetOptions options{};
	Cesium3DTilesSelection::TilesetOptions options;
	options.maximumScreenSpaceError = 16.0;
	tileset = std::make_shared<Cesium3DTilesSelection::Tileset>(*tilesetExternals,
		path_tileset,
		options);
	//Cesium3DTilesSelection::Tile* pTilesetJson = const_cast<Cesium3DTilesSelection::Tile*>(tileset->getRootTile());
	////pTilesetJson->setTransform(enuMatrix);
	//std::cout << "Root tile children count: " << tileset->getRootTile()->getChildren().size() << std::endl;
	//std::cout << "Root geometric error: " << tileset->getRootTile()->getGeometricError() << std::endl;

	/*HARD CODE TEST LOADER TILESET*/
	//glm::dmat4 enuMatrix = CesiumGeospatial::GlobeTransforms::eastNorthUpToFixedFrame(ecef);
	//std::string tilesetPathStr = path_tileset; 
	//auto loaderResultFuture = Cesium3DTilesSelection::TilesetJsonLoader::createLoader(*tilesetExternals, tilesetPathStr, {});
	//tilesetExternals->asyncSystem.dispatchMainThreadTasks();
	//auto loaderResult = loaderResultFuture.wait();
	//auto pTilesetJson = loaderResult.pRootTile.get();
	//pTilesetJson->setTransform(enuMatrix); 
	//auto pRootTile = &pTilesetJson->getChildren()[0];
	//pRootTile->setTransform(enuMatrix); 
	//tileset = std::make_shared<Cesium3DTilesSelection::Tileset>(*tilesetExternals,
	//	path_tileset); 
	//auto children = pRootTile->getChildren();
}

CesiumDrawableStyleLayerHost::~CesiumDrawableStyleLayerHost()
{
}

void CesiumDrawableStyleLayerHost::initialize()
{
}

Cesium3DTilesSelection::ViewState CesiumDrawableStyleLayerHost::createViewState2(Interface& interface)
{
	const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
	const mbgl::TransformState& state = interface.state; 
	auto tmp = state.getCamera().up();
	const mbgl::LatLng& center = state.getLatLng();
	double center_altitude = 0.0;
	CesiumGeospatial::Cartographic radian_center_location = CesiumGeospatial::Cartographic::fromDegrees(center.longitude(), center.latitude(), center_altitude);
	glm::dvec3 ecef_center = ellipsoid.cartographicToCartesian(radian_center_location);

	const mbgl::FreeCameraOptions& free_cam_options = state.getFreeCameraOptions();
	const mbgl::LatLng& location_cam_wgs84 = free_cam_options.getLocation().value().location;//lat lon 
	const double& cam_altitude_wgs84 = free_cam_options.getLocation().value().altitude; //
	CesiumGeospatial::Cartographic radian_camera_location = CesiumGeospatial::Cartographic::fromDegrees(location_cam_wgs84.longitude(), location_cam_wgs84.latitude(), cam_altitude_wgs84);
	glm::dvec3 ecef_camera_location = ellipsoid.cartographicToCartesian(radian_camera_location);
	auto carto_camera = ellipsoid.cartesianToCartographic(ecef_camera_location);
	glm::dvec3 worldUp = ellipsoid.geodeticSurfaceNormal(ecef_camera_location);
	// Viewport và FOV
	double aspectRatio = state.getSize().aspectRatio();
	glm::dvec2 viewPortSize = glm::dvec2(state.getSize().width, state.getSize().height);
	double horizontalFieldOfView = CesiumUtility::Math::degreesToRadians(50); /* state.getFieldOfView();*/
	double verticalFieldOfView = std::atan(std::tan(horizontalFieldOfView * 0.5) / aspectRatio) * 2.0;
	glm::dvec3 direction = glm::normalize(ecef_center - ecef_camera_location);
	glm::dvec3 right = glm::normalize(glm::cross(direction, worldUp)); 
	glm::dvec3 vUp = glm::cross(right, direction); 
	return Cesium3DTilesSelection::ViewState(
		ecef_camera_location,
		direction,
		vUp,
		viewPortSize,
		horizontalFieldOfView,
		verticalFieldOfView,
		ellipsoid);
}

Cesium3DTilesSelection::ViewState CesiumDrawableStyleLayerHost::createViewState(Interface& interface)
{
	const CesiumGeospatial::Ellipsoid& ellipsoid = CesiumGeospatial::Ellipsoid::WGS84;
	const mbgl::TransformState& state = interface.state;
	const mbgl::util::Camera& camera = state.getCamera();
	//vector up
	auto bearing = state.getBearing(); 
	auto pitch = state.getPitch(); 
	const mbgl::vec3& up = camera.up();
	//center
	const mbgl::LatLng& center = state.getLatLng();
	//forward 
	const mbgl::vec3& forward = camera.forward(); 
	CesiumGeospatial::Cartographic radian_center_location = CesiumGeospatial::Cartographic::fromDegrees(center.longitude(), center.latitude(), 0.0);
	glm::dvec3 ecef_center = ellipsoid.cartographicToCartesian(radian_center_location); 
	const mbgl::FreeCameraOptions& free_cam_options = state.getFreeCameraOptions();
	const mbgl::LatLng& location_cam_wgs84 = free_cam_options.getLocation().value().location;//lat lon 
	const double& cam_altitude_wgs84 = free_cam_options.getLocation().value().altitude; //
	CesiumGeospatial::Cartographic radian_camera_location = CesiumGeospatial::Cartographic::fromDegrees(location_cam_wgs84.longitude(), location_cam_wgs84.latitude(), cam_altitude_wgs84);
	glm::dvec3 ecef_camera_location = ellipsoid.cartographicToCartesian(radian_camera_location);
	glm::dmat4 enuToEcef = CesiumGeospatial::GlobeTransforms::eastNorthUpToFixedFrame(ecef_camera_location);
	double aspectRatio = state.getSize().aspectRatio();
	glm::dvec3 direction_t = glm::normalize(ecef_center - ecef_camera_location); //viewFocus - ViewPostion
	glm::dvec3 direction = glm::dvec3(forward[0], forward[1], forward[2]); 
	glm::dvec3 viewUp = glm::dvec3(camera.up()[0], camera.up()[1], camera.up()[2]);
	glm::dvec2 viewPortSize = glm::dvec2(state.getSize().width,state.getSize().height); 
	double horizontalFieldOfView = state.getFieldOfView(); 
	double verticalFieldOfView =
		std::atan(std::tan(horizontalFieldOfView * 0.5) / aspectRatio) * 2.0;
	glm::dvec3 forwardECEF = glm::normalize(glm::dvec3(enuToEcef * glm::dvec4(direction, 0.0)));
	glm::dvec3 upECEF = glm::normalize(glm::dvec3(enuToEcef * glm::dvec4(viewUp, 0.0)));
	return Cesium3DTilesSelection::ViewState(
		ecef_camera_location,
		direction_t,
		upECEF,
		viewPortSize,
		horizontalFieldOfView,
		verticalFieldOfView,
		ellipsoid);
}

void CesiumDrawableStyleLayerHost::update(Interface& interface)
{
	Cesium3DTilesSelection::ViewState viewstate = createViewState2(interface);
	Cesium3DTilesSelection::ViewUpdateResult result = tileset->updateView(
		{ viewstate }
	);
	//tileset->loadedTiles(); 
	std::cout << std::format("Tile render this frame : {}", result.tilesToRenderThisFrame.size()) << std::endl; 
}

void CesiumDrawableStyleLayerHost::deinitialize()
{
}
