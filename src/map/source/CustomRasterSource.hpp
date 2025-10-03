#ifndef _MAPLIBRE_CUSTOM_RASTER_SOURCE_HPP_
#define _MAPLIBRE_CUSTOM_RASTER_SOURCE_HPP_
#include <core/utils/LRUCache11.hpp>
#include <threadpool/BS_thread_pool.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <unordered_set>
#include <string>
namespace threepp
{
	class Texture;
};
class WorkerManager
{
public: 
	void createWorker(const mbgl::CanonicalTileID& tileID)
	{
		std::lock_guard<std::mutex> guard(lock);
		{
			worker_manager.insert({ tileID });
		}
	}
	bool containsWorker(const mbgl::CanonicalTileID& tileID)
	{
		std::lock_guard<std::mutex> guard(lock);
		{
			return worker_manager.count(tileID) != 0;
		}
	}
	void releaseWorker(const mbgl::CanonicalTileID& tileID)
	{
		std::lock_guard<std::mutex> guard(lock);
		{
			worker_manager.erase(tileID);
		}
	}
private:
	std::unordered_set<mbgl::CanonicalTileID> worker_manager{}; 
	std::mutex lock{}; 
};

class MaplibreCustomRasterSource
{
	
public: 
	MaplibreCustomRasterSource(const std::string url, const std::string& token); 
	~MaplibreCustomRasterSource(); 
public: 
	void RegisterDownloadTile(const mbgl::CanonicalTileID& tileID);
public: 
	std::shared_ptr<lru11::Cache<mbgl::CanonicalTileID, std::shared_ptr<threepp::Texture>,std::mutex>> raster_cache{ nullptr };
	BS::thread_pool<BS::tp::none> pool{ 24 };
	size_t lru_size{ 5000 }; 
	WorkerManager workerManager{}; 
	std::string token{ "" }; 
	std::string url{ "" }; 
};
#endif // !_MAPLIBRE_CUSTOM_SOURCE_HPP_
