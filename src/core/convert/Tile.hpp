#pragma once
#ifndef _TILE_HPP_
#define _TILE_HPP_
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
#include <mbgl/util/constants.hpp>
namespace Convert
{
	struct TileCoord {
		int tileX;
		int tileY;
        int tileZ; 
		/*double localX;
		double localY;*/
        glm::dvec2 localCoord; 
	};

    struct LocalCoord {
        double localX;
        double localY;
    };

    static TileCoord wgs84ToTile(double lon, double lat, int zoom = 16, int tileSize = 512, int tileExtent = 8192) {
        double mapSize = double(tileSize) * (1 << zoom);

        double pixelX = (lon + 180.0) / 360.0 * mapSize;
        double sinLat = sin(glm::radians(lat));
        double pixelY = (0.5 - log((1 + sinLat) / (1 - sinLat)) / (4 * M_PI)) * mapSize;

        int tileX = int(pixelX) / tileSize;
        int tileY = int(pixelY) / tileSize;
         
        double localX = fmod(pixelX, tileSize) / tileSize * tileExtent;
        double localY = fmod(pixelY, tileSize) / tileSize * tileExtent;

        return { tileX, tileY, zoom, glm::dvec2(localX, localY) };
    }

    static LocalCoord wgs84ToLocalInTile(double lon, double lat, int tileX, int tileY,
        int zoom = 16, int tileSize = 512, int tileExtent = 8192) {
        lon = glm::clamp(lon, -180.0, 180.0);
        lat = glm::clamp(lat, -85.051128779806592, 85.051128779806592); // Web Mercator limits

        double mapSize = double(tileSize) * (1 << zoom);

        // Global pixel coordinates
        double pixelX = (lon + 180.0) / 360.0 * mapSize;

        double latRad = glm::radians(lat);
        double sinLat = sin(latRad);

        // Safe calculation to avoid numerical issues
        double mercatorY = 0.5 - log((1 + sinLat) / (1 - sinLat)) / (4 * M_PI);
        double pixelY = mercatorY * mapSize;

        // Origin of given tile (top-left corner)
        double tileOriginX = tileX * tileSize;
        double tileOriginY = tileY * tileSize;

        // Local coordinates relative to this tile
        double localX = (pixelX - tileOriginX) / tileSize * tileExtent;
        double localY = (pixelY - tileOriginY) / tileSize * tileExtent;

        return { localX, localY };
    }

    static double computeScaleZForLevel(const uint8_t zoomlevel)
    {
        const double tileSize = mbgl::util::tileSize_D;
        const double tileExtent = mbgl::util::EXTENT;
        const double earthCircumference = 2.0 * M_PI * mbgl::util::EARTH_RADIUS_M; // WGS84
        uint64_t numTiles = 1ULL << zoomlevel;
        double realTileSizeInMeters = earthCircumference / numTiles;
        double metersPerPixel = realTileSizeInMeters / tileSize;
        double scale = metersPerPixel * (tileExtent / tileSize);
        return 1.0 / scale;
    }

}
#endif // !_TILE_HPP_
