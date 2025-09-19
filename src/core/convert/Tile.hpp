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

    static std::pair<double, double> tileToLatLon(int x, int y, int z) {
        double n = std::pow(2.0, z);
        double lon = x / n * 360.0 - 180.0;
        double lat_rad = std::atan(std::sinh(M_PI * (1 - 2.0 * y / n)));
        double lat = lat_rad * 180.0 / M_PI;
        return { lat, lon };
    }

    static glm::dvec2 pixel_to_latlon(int tileX, int tileY, int zoom, double px, double py, int extent = 8192) {
        double n = M_PI - 2.0 * M_PI * ((py / extent + tileY) / pow(2.0, zoom));
        double lat = 180.0 / M_PI * atan(0.5 * (exp(n) - exp(-n)));
        double lon = ((px / extent + tileX) / pow(2.0, zoom)) * 360.0 - 180.0;
        return glm::dvec2(lat, lon);
    }

    static glm::dvec2 latlon_to_pixel(int tileX, int tileY, int zoom, double lat, double lon, int extent = 8192) {
        double x = (lon + 180.0) / 360.0 * pow(2.0, zoom);
        double y = (1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0 * pow(2.0, zoom);
        double local_x = (x - tileX) * extent;
        double local_y = (y - tileY) * extent;
        return glm::dvec2(local_x, local_y);
    }

    static glm::dmat4 lonlatToLocalTileMatrix(int tileX, int tileY, int zoom, int tileSize, int tileExtent) {
        double mapSize = double(tileSize) * (1 << zoom);

        // Tâm tile (lon0, lat0)
        double lon0 = (double(tileX) + 0.5) / (1 << zoom) * 360.0 - 180.0;
        double n = M_PI - 2.0 * M_PI * (double(tileY) + 0.5) / (1 << zoom);
        double lat0 = glm::degrees(atan(0.5 * (exp(n) - exp(-n))));

        // Scale factor
        double scale = double(tileExtent) / tileSize;

        // Derivatives
        double dX_dLon = mapSize / 360.0 * scale;
        double dY_dLat = -(mapSize / (2 * M_PI)) * scale / cos(glm::radians(lat0));

        // Local offset for tile origin
        double tileOriginX = tileX * tileSize;
        double tileOriginY = tileY * tileSize;

        double pixelOriginX = (lon0 + 180.0) / 360.0 * mapSize;
        double mercY0 = 0.5 - log((1 + sin(glm::radians(lat0))) / (1 - sin(glm::radians(lat0)))) / (4 * M_PI);
        double pixelOriginY = mercY0 * mapSize;

        double localOriginX = (pixelOriginX - tileOriginX) / tileSize * tileExtent;
        double localOriginY = (pixelOriginY - tileOriginY) / tileSize * tileExtent;

        glm::dmat4 M(1.0);
        M[0][0] = dX_dLon;
        M[1][1] = dY_dLat;
        M[3][0] = localOriginX - dX_dLon * lon0;
        M[3][1] = localOriginY - dY_dLat * lat0;

        return M;
    }


}
#endif // !_TILE_HPP_
