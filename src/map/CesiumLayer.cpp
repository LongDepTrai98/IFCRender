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
#include <glm/gtc/type_ptr.hpp>
#include "CesiumLayer.hpp"
#include "cesium/ThreadTaskProcessor.hpp"
#include "cesium/SimplePrepareRendererResource.hpp"
#include "cesium/SimpleTaskProcessor.hpp"
#include "cesium/SimpleAssetAccessor.hpp"
#include "threepp/threepp.hpp"
#include "TilesetJsonLoader.h"
#include "core/utils/CesiumHelper.hpp"
#include "core/convert/Tile.hpp"

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


	double rw = -1.3197209591796106;
	double rs = 0.6988424218;
	double re = -1.3196390408203893;
	double rn = 0.6989055782;
	double mh = 0;
	double maxh = 88;

	double ww = CesiumUtility::Math::radiansToDegrees(rw);
	double ws = CesiumUtility::Math::radiansToDegrees(rs);
	double we = CesiumUtility::Math::radiansToDegrees(re);
	double wn = CesiumUtility::Math::radiansToDegrees(rn);

	double center_test_lon = (ww + we) * 0.5f; 
	double center_test_lat = (ws + wn) * 0.5f; 

	glm::dvec3 ecef = dragon::CesiumHelper::wgs84ToEcef(center_test_lon,center_test_lat,0.0);
	glm::dmat4 enuMatrix = CesiumGeospatial::GlobeTransforms::eastNorthUpToFixedFrame(ecef);
	printMatrix(enuMatrix);
	mockAssetAccessor = std::make_shared<CesiumNativeTests::SimpleAssetAccessor>(); 
	tilesetExternals = std::make_shared<Cesium3DTilesSelection::TilesetExternals>(
		mockAssetAccessor,
		std::make_shared<Cesium3DTilesSelection::SimplePrepareRendererResource>(),
		CesiumAsync::AsyncSystem(std::make_shared<CesiumNativeTests::ThreadTaskProcessor>()),
		nullptr
	); 
	std::string path_tileset{"D:/GITHUB/IFCRender/cesium-native/Cesium3DTilesSelection/test/data/ReplaceTileset/tileset.json" }; 
	Cesium3DTilesSelection::TilesetOptions options;
	options.maximumScreenSpaceError = 16.0;
	tileset = std::make_shared<Cesium3DTilesSelection::Tileset>(*tilesetExternals,
		path_tileset,
		options);

	fnc_create_drawable = [&](Interface& interface, const Cesium3DTilesSelection::BoundingVolume& BoundingVolume)
	{
		glm::dvec3 ecef_center_bounding_volume = dragon::CesiumHelper::getCenterBoundingVolume(BoundingVolume);
		std::optional<glm::dvec3> wgs84_center_bounding_volume = dragon::CesiumHelper::ecefToWgs84(ecef_center_bounding_volume);
		const double lon = wgs84_center_bounding_volume.value().x;
		const double lat = wgs84_center_bounding_volume.value().y;
		auto tile = Convert::wgs84ToTile(lon, lat);
		interface.addCustomDrawableWithTile({ (uint8_t)tile.tileZ, (uint32_t)tile.tileX, (uint32_t)tile.tileY });
	}; 

}

CesiumDrawableStyleLayerHost::~CesiumDrawableStyleLayerHost()
{
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
	double horizontalFieldOfView = CesiumUtility::Math::degreesToRadians(50); /* state.getFieldOfView();*/
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
	auto& state = interface.state; 
	Cesium3DTilesSelection::ViewState viewstate = createViewState(interface);
	Cesium3DTilesSelection::ViewUpdateResult result = tileset->updateView(
		{ viewstate }
	);
	auto tile_root = tileset->getRootTile();
	if (fnc_create_drawable && tile_root->getState() == Cesium3DTilesSelection::TileLoadState::Done)
	{
		//hard code get center
		const Cesium3DTilesSelection::BoundingVolume& BoundingVolume = tile_root->getBoundingVolume();
		fnc_create_drawable(interface,BoundingVolume);
		fnc_create_drawable = nullptr; 
	}
	std::cout << std::format("Tile render this frame : {}", result.tilesToRenderThisFrame.size()) << std::endl; 
}

void CesiumDrawableStyleLayerHost::deinitialize()
{
}



