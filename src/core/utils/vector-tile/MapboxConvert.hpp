#pragma once
#ifndef _MAPBOX_CONVERT_HPP_
#define _MAPBOX_CONVERT_HPP_
#include "Tile.hpp"
#include "TileSystem.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
namespace mapbox
{
	class Convert
	{
	public:
		Convert(double lon, double lat, int level)
		{
			current_tile =  Tile(lon, lat, level);
			int tile_x = current_tile.x_;
			int tile_y = current_tile.y_;
			long long pixel_tile_X, pixel_tile_Y;
			TileSystem::TileXYToPixelXY(tile_x, tile_y, pixel_tile_X, pixel_tile_Y);
			long long pixel_X;
			long long pixel_Y;
			TileSystem::LatLongToPixelXY(lat, lon, level, pixel_X, pixel_Y);
			coord_x = (pixel_X - pixel_tile_X) * (1.0 / 512) * extent;
			coord_y = (pixel_Y - pixel_tile_Y) * (1.0 / 512) * extent;
		}; 
		glm::dvec2 getPixelCoord()
		{
			long long pixelX = extent * current_tile.x_ + coord_x; 
			long long pixelY = extent * current_tile.y_ + coord_y; 
			return { pixelX,pixelY }; 
		}
		std::string getTileName()
		{
			return std::to_string(current_tile.z_) + "_" + std::to_string(current_tile.x_) + "_" + std::to_string(current_tile.y_); 
		}
		~Convert() = default;

	public:
		int extent = 8192;
		int coord_x; 
		int coord_y; 
		Tile current_tile; 
	};
}
#endif // !_MAPBOX_CONVERT_HPP_
