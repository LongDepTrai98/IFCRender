#pragma once

#pragma warning(disable: 4244)

#ifndef ____CONST_CRS____
#define ____CONST_CRS____
#include <cmath>
#include <cstdint>

const double TileSize		= 512;
const double RadToDeg		= 57.295779513082320876798154814105;
const double EarthRadius	= 6378137.0;
const double MinLatitude	= -85.05112878;
const double MaxLatitude	= 85.05112878;
const double MinLongitude	= -180;
const double MaxLongitude	= 180;
const double So_PI			= 3.141592653589793238462643383279;

#endif

// MBR VietNam: 102.144583, 7.391438, 117.817345, 23.393692
// MBR TP.HCM: 106.356421, 10.376714, 107.012798, 11.160291

class TileSystem
{
public:
	TileSystem(){}
	~TileSystem(){}

private:
	static double Clip(const double &n, const double &minValue, const double &maxValue)
	{
		return __min(__max(n, minValue), maxValue);
	}

public:
	static __int64 MapSize(const int &nLevelZoom)
	{
		return (__int64)TileSize << nLevelZoom;
	}

	static __int64 MapSizeInTile(const int&nLevelZoom)
	{
		return (__int64)1 << nLevelZoom;
	}

	static double GroundResolution(double latitude, const int&nLevelZoom)
	{
		latitude = Clip(latitude, MinLatitude, MaxLatitude);
		return cos(latitude * So_PI / 180) * 2 * So_PI * EarthRadius / MapSize(nLevelZoom);
	}

	static double MapScale(const double &latitude, const int&nLevelZoom, const int&screenDpi)
	{
		return GroundResolution(latitude, nLevelZoom) * screenDpi / 0.0254;
	}

	static void LatLongToPixelXY(const double& latitude, const double& longitude, const int& nLevelZoom, float& pixelX, float& pixelY)
	{
		double x = (longitude + 180) / 360;
		double sinLatitude = sin(latitude * So_PI / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * So_PI);

		__int64 mapSize = MapSize(nLevelZoom);
		pixelX = Clip(x * mapSize, 0, mapSize - 1);
		pixelY = Clip(y * mapSize, 0, mapSize - 1);
	}

	static void LatLongToPixelXY(const double &latitude, const double &longitude, const int& nLevelZoom, double& pixelX, double& pixelY)
	{
		double x = (longitude + 180) / 360;
		double sinLatitude = sin(latitude * So_PI / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * So_PI);

		__int64 mapSize = MapSize(nLevelZoom);
		pixelX = Clip(x * mapSize, 0, mapSize - 1);
		pixelY = Clip(y * mapSize, 0, mapSize - 1);
	}


	static void LatLongToPixelXY(double latitude, double longitude, const int& nLevelZoom, int64_t& pixelX, int64_t& pixelY)
	{
		latitude = Clip(latitude, MinLatitude, MaxLatitude);
		longitude = Clip(longitude, MinLongitude, MaxLongitude);

		double x = (longitude + 180) / 360;
		double sinLatitude = sin(latitude * So_PI / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * So_PI);

		__int64 mapSize = MapSize(nLevelZoom);
		pixelX = Clip(x * mapSize + 0.5, 0, mapSize - 1);
		pixelY = Clip(y * mapSize + 0.5, 0, mapSize - 1);
	}

	static void LatLongToPixelXY(double latitude, double longitude, const int &nLevelZoom, long& pixelX, long& pixelY)
	{
		latitude = Clip(latitude, MinLatitude, MaxLatitude);
		longitude = Clip(longitude, MinLongitude, MaxLongitude);

		double x = (longitude + 180) / 360;
		double sinLatitude = sin(latitude * So_PI / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * So_PI);

		__int64 mapSize = MapSize(nLevelZoom);
		pixelX = (long)Clip(x * mapSize + 0.5, 0, mapSize - 1);
		pixelY = (long)Clip(y * mapSize + 0.5, 0, mapSize - 1);
	}

	static void PixelXYToLatLong(const double& pixelX, const double& pixelY, long nLevelZoom, double& latitude, double& longitude)
	{
		double mapSize = MapSize(nLevelZoom);
		double x = (Clip(pixelX, 0, mapSize - 1) / mapSize) - 0.5;
		double y = 0.5 - (Clip(pixelY, 0, mapSize - 1) / mapSize);

		latitude = 90 - 360.0f * atan(exp(-y * 2 * So_PI)) / So_PI;
		longitude = 360.0f * x;
	}

	static void PixelXYToLatLong(const __int64& pixelX, const __int64& pixelY, long nLevelZoom, double& latitude, double& longitude)
	{
		double mapSize = MapSize(nLevelZoom);
		double x = (Clip(pixelX, 0, mapSize - 1) / mapSize) - 0.5;
		double y = 0.5 - (Clip(pixelY, 0, mapSize - 1) / mapSize);

		latitude = 90 - 360 * atan(exp(-y * 2 * So_PI)) / So_PI;
		longitude = 360 * x;
	}

	static void PixelXYToTileXY(const __int64 &pixelX, const __int64&pixelY, double& tileX, double& tileY)
	{
		tileX = pixelX / TileSize;
		tileY = pixelY / TileSize;
	}

	static void TileXYToPixelXY(const long &tileX, const long &tileY, __int64& pixelX, __int64& pixelY)
	{
		pixelX = static_cast<__int64>(tileX) * TileSize;
		pixelY = static_cast<__int64>(tileY) * TileSize;
	}

	static void LatLongToTileXY(double latitude, double longitude, const int &nLevelZoom, double& tileX, double& tileY)
	{
		latitude = Clip(latitude, MinLatitude, MaxLatitude);
		longitude = Clip(longitude, MinLongitude, MaxLongitude);

		double x = (longitude + 180) / 360;
		double sinLatitude = sin(latitude * So_PI / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * So_PI);

		__int64 mapSize = MapSize(nLevelZoom);
		long pixelX = (long)Clip(x * mapSize + 0.5, 0, mapSize - 1);
		long pixelY = (long)Clip(y * mapSize + 0.5, 0, mapSize - 1);

		tileX = pixelX / TileSize;
		tileY = pixelY / TileSize;
	}

	static void LatLongToTileXY(double latitude, double longitude, const int& nLevelZoom, long& tileX, long& tileY)
	{
		latitude = Clip(latitude, MinLatitude, MaxLatitude);
		longitude = Clip(longitude, MinLongitude, MaxLongitude);

		double x = (longitude + 180) / 360;
		double sinLatitude = sin(latitude * So_PI / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * So_PI);

		__int64 mapSize = MapSize(nLevelZoom);
		long pixelX = (long)Clip(x * mapSize + 0.5, 0, mapSize - 1);
		long pixelY = (long)Clip(y * mapSize + 0.5, 0, mapSize - 1);

		tileX = pixelX / TileSize;
		tileY = pixelY / TileSize;
	}


	static void TileXYToBound(long nLevelZoom, const long &tileX, const long &tileY, double &minLat, double &minLon, double &maxLat, double &maxLon)
	{
		__int64 pixelMinX = tileX * TileSize;
		__int64 pixelMinY = tileY * TileSize;
		__int64 pixelMaxX = (tileX + 1) * TileSize;
		__int64 pixelMaxY = (tileY + 1) * TileSize;

		PixelXYToLatLong(pixelMinX, pixelMinY, nLevelZoom, maxLat, minLon);
		PixelXYToLatLong(pixelMaxX, pixelMaxY, nLevelZoom, minLat, maxLon);
	}

	static void GetBound(long nLevelZoom, long tileX, long tileY, double& latMin, double& lonMin, double& latMax, double& lonMax)
	{
		//double powZoom = MapSizelongile(nLevelZoom);
		//double lonDegPerPix = 360.0 / (TileSize * powZoom);

		//latMax = atan(sinh(So_PI * (1 - 2 * (tileY) / powZoom))) * RadToDeg;	//NW corner
		//latMin = atan(sinh(So_PI * (1 - 2 * (tileY + 1) / powZoom))) * RadToDeg;	//SE corner

		//lonMin = (tileX / powZoom) * 360.0 - 180.0;
		//lonMax = lonMin + lonDegPerPix * TileSize;
	}

	static void GetBound(long nnLevelZoom, long nTileX, long nTileY, long nRadius, double& dLatMin, double& dLonMin, double& dLatMax, double& dLonMax, long& nWidth, long& nHeight)
	{
		//double dPowZoom = MapSizelongile(nnLevelZoom);
		//double dLonDegPerPix = 360.0 / (TileSize * dPowZoom);
		//long nTileXMin = std::max<long>(0, nTileX - nRadius), nTileXMax = std::min<long>(dPowZoom, nTileX + nRadius);
		//long nTileYMin = std::max<long>(0, nTileY - nRadius), nTileYMax = std::min<long>(dPowZoom, nTileY + nRadius);
		//long nXTiles = nTileXMax - nTileXMin, nYTiles = nTileYMax - nTileYMin;

		//nWidth = TileSize * nXTiles;
		//nHeight = TileSize * nYTiles;

		//dLatMax = atan(sinh(So_PI * (1 - 2 * (nTileYMin) / dPowZoom))) * RadToDeg;	//NW corner
		//dLatMin = atan(sinh(So_PI * (1 - 2 * (nTileYMin + nYTiles) / dPowZoom))) * RadToDeg;	//SE corner

		//dLonMin = (nTileXMin / dPowZoom) * 360.0 - 180.0;
		//dLonMax = dLonMin + dLonDegPerPix * TileSize * nXTiles;
	}

	static void GetTiles(const double &latMin, const double& lonMin, const double& latMax, const double& lonMax, const long& nLevelZoom,
						long& tileXMin, long& tileYMin, long& tileXMax, long& tileYMax)
	{
		LatLongToTileXY(latMax, lonMin, nLevelZoom, tileXMin, tileYMin);
		LatLongToTileXY(latMin, lonMax, nLevelZoom, tileXMax, tileYMax);
	}

	static void GetTiles(double latMin, double lonMin, double latMax, double lonMax, long nLevelZoom,
						long& tileXMin, long& tileYMin, long& tileXMax, long& tileYMax,
						long& pixelXMin, long& pixelYMin, long& pixelXMax, long& pixelYMax)
	{
		// latitude1, longitude1
		latMin = Clip(latMin, MinLatitude, MaxLatitude);
		lonMin = Clip(lonMin, MinLongitude, MaxLongitude);

		double x = (lonMin + 180) / 360;
		double sinLatitude = sin(latMin * So_PI / 180);
		double y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * So_PI);

		__int64 mapSize = MapSize(nLevelZoom);
		long pixelX = (long)Clip(x * mapSize + 0.5, 0, mapSize - 1);
		long pixelY = (long)Clip(y * mapSize + 0.5, 0, mapSize - 1);

		tileXMin = pixelX / TileSize;
		tileYMin = pixelY / TileSize;
		pixelXMin = pixelX % (long)TileSize;
		pixelYMin = pixelY % (long)TileSize;

		// latitude2, longitude2
		latMax = Clip(latMax, MinLatitude, MaxLatitude);
		lonMax = Clip(lonMax, MinLongitude, MaxLongitude);

		x = (lonMax + 180) / 360;
		sinLatitude = sin(latMax * So_PI / 180);
		y = 0.5 - log((1 + sinLatitude) / (1 - sinLatitude)) / (4 * So_PI);

		pixelX = (long)Clip(x * mapSize + 0.5, 0, mapSize - 1);
		pixelY = (long)Clip(y * mapSize + 0.5, 0, mapSize - 1);

		tileXMax = pixelX / TileSize;
		tileYMax = pixelY / TileSize;
		pixelXMax = pixelX % (long)TileSize;
		pixelYMax = pixelY % (long)TileSize;
	}

	static void PointVectorTileToPixelXY(	const double& x, const double& y, 
											__int64& pixelX, __int64& pixelY,
											const double& extent, const int& level, 
											const int& tileX, const int& tileY)
	{
		__int64 pX, pY;

		TileXYToPixelXY(tileX, tileY, pX, pY);

		pixelX = (double)pX + (x / extent) * 256.0;
		pixelY = (double)pY + (y / extent) * 256.0;
	}
};
