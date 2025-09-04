#pragma once
#ifndef _TILE_HPP_
#define _TILE_HPP_
#include <cmath>
#include <iostream>
#include <glm/glm.hpp>
namespace Convert
{
	struct TileCoord {
		int tileX;
		int tileY;
        int tileZ; 
		double localX;
		double localY;
	};

    struct LocalCoord {
        double localX;
        double localY;
    };

    TileCoord wgs84ToTile(double lon, double lat, int zoom = 16, int tileSize = 512, int tileExtent = 8192) {
        double mapSize = double(tileSize) * (1 << zoom);

        double pixelX = (lon + 180.0) / 360.0 * mapSize;
        double sinLat = sin(glm::radians(lat));
        double pixelY = (0.5 - log((1 + sinLat) / (1 - sinLat)) / (4 * M_PI)) * mapSize;

        int tileX = int(pixelX) / tileSize;
        int tileY = int(pixelY) / tileSize;
         
        double localX = fmod(pixelX, tileSize) / tileSize * tileExtent;
        double localY = fmod(pixelY, tileSize) / tileSize * tileExtent;

        return { tileX, tileY, zoom, localX, localY };
    }

    LocalCoord wgs84ToLocalInTile(double lon, double lat, int tileX, int tileY,
        int zoom = 16, int tileSize = 512, int tileExtent = 8192) {
        double mapSize = double(tileSize) * (1 << zoom);

        // Global pixel
        double pixelX = (lon + 180.0) / 360.0 * mapSize;
        double sinLat = sin(glm::radians(lat));
        double pixelY = (0.5 - log((1 + sinLat) / (1 - sinLat)) / (4 * M_PI)) * mapSize;

        // Origin of given tile
        double tileOriginX = tileX * tileSize;
        double tileOriginY = tileY * tileSize;

        // Local coordinates relative to this tile
        double localX = (pixelX - tileOriginX) / tileSize * tileExtent;
        double localY = (pixelY - tileOriginY) / tileSize * tileExtent;

        return { localX, localY };
    }

}
#endif // !_TILE_HPP_
