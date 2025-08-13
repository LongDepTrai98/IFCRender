#pragma once
#ifndef _TILE_HPP_H
#define _TILE_HPP_H
class Tile
{
public: 
	Tile(double lng, double lat, long z); 
	Tile(); 
	~Tile() = default;
public: 
    bool operator==(const Tile& other) const
    {
        return z_ == other.z_ && x_ == other.x_ && y_ == other.y_;
    }

    bool operator<(const Tile& other) const
    {
        if (z_ < other.z_) return true;

        if (z_ == other.z_)
        {
            if (x_ < other.x_)
                return true;
            else
            {
                if (x_ == other.x_)
                {
                    if (y_ < other.y_)
                        return true;
                }
            }
        }

        return false;
    }
public: 
	long x_; 
	long y_; 
	long z_; 
};
#endif // !_TILE_HPP_H

