#pragma once
#ifndef _CESIUM_HELPER_HPP_
#define _CESIUM_HELPER_HPP_
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <optional>
#include <Cesium3DTilesSelection/BoundingVolume.h>
namespace threepp
{
	class Group;
}
namespace dragon
{
	class CesiumHelper
	{
	public: 
		static std::shared_ptr<threepp::Group> createGLB(std::vector<std::byte>& bytes);
		static glm::dvec3 wgs84ToEcef(const double& lon, const double& lat, const double& height);
		static std::optional<glm::dvec3> ecefToWgs84(glm::dvec3 ecef); 
		static glm::dvec3 getCenterBoundingVolume(const Cesium3DTilesSelection::BoundingVolume& BoundingVolume);
		static double getMetersPerExtentUnit(double lat, int zoom = 16); 
	};
}
#endif // !_CESIUM_HELPER_HPP_