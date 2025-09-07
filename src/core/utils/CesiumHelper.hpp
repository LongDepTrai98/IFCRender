#pragma once
#ifndef _CESIUM_HELPER_HPP_
#define _CESIUM_HELPER_HPP_
#include <CesiumGltf/Model.h>
#include <Cesium3DTilesSelection/BoundingVolume.h>
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <optional>
#include <mbgl/tile/tile_id.hpp>
namespace threepp
{
	class Group;
	class Mesh; 
}
namespace dragon
{
	class CesiumHelper
	{
	public: 
		struct B3DMExtensions {
			glm::dvec3 rtcCenter = { 0.0, 0.0, 0.0 };
			bool hasRTC = false;
		};
	public: 
		static std::shared_ptr<threepp::Group> createGLB(std::vector<std::byte>& bytes);
		static std::shared_ptr<threepp::Group> createMesh(CesiumGltf::Model& model, B3DMExtensions& extension);
		static glm::dvec3 wgs84ToEcef(const double& lon, const double& lat, const double& height);
		static std::optional<glm::dvec3> ecefToWgs84(glm::dvec3 ecef); 
		static glm::dvec3 getCenterBoundingVolume(const Cesium3DTilesSelection::BoundingVolume& BoundingVolume);
		static double calculateECEFMercatorTilt(const glm::dvec3& ecefPoint);
		static double getMetersPerExtentUnit(double lat);
		static std::shared_ptr<threepp::Mesh> createOrientedBoundingBox(Cesium3DTilesSelection::BoundingVolume& boundingVolume,const mbgl::CanonicalTileID& tileID); 
	};
}
#endif // !_CESIUM_HELPER_HPP_