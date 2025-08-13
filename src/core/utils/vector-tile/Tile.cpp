#include "TileSystem.hpp"
#include "Tile.hpp"
#include <iostream>
Tile::Tile(double lng, double lat, long z)
	: z_(z)
{
	//lay x,y,z cua lon,lat su dung tile system
	long y; 
	TileSystem::LatLongToTileXY(lat,
		lng,
		z,
		x_,
		y_);
	std::cout << "Tile x: " << x_ << std::endl; 
	std::cout << "Tile y: " << y_ << std::endl; 
	std::cout << "Tile z: " << z_ << std::endl; 
}

Tile::Tile()
	: x_(-1),
	y_(-1),
	z_(-1)
{
	//set default cho tile 
}
