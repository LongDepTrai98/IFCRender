#include "threepp/threepp.hpp"
#include "CustomRasterSource.hpp"
#include <mbgl/util/image.hpp>
#include <format>
#include <cpr/cpr.h>
#include <spdlog/spdlog.h>

MaplibreCustomRasterSource::MaplibreCustomRasterSource(const std::string url_,
	const std::string& token_) : token(token_), 
	url(url_)
{
	if (!raster_cache)
		raster_cache = std::make_shared< lru11::Cache<mbgl::CanonicalTileID, std::shared_ptr<threepp::Texture>,std::mutex>>(lru_size, 10);
}
MaplibreCustomRasterSource::~MaplibreCustomRasterSource()
{
	if(raster_cache)
		raster_cache->clear(); 
	pool.wait(); 
}
void MaplibreCustomRasterSource::RegisterDownloadTile(const mbgl::CanonicalTileID& tileID)
{
	if (!raster_cache->contains(tileID))
	{
		if (!workerManager.containsWorker(tileID))
		{
			workerManager.createWorker(tileID);

			auto f = [this, tileID]() {
				//hard code url 
				std::string formatUrl = std::format("https://api.mapbox.com/v4/mapbox.satellite/{}/{}/{}@2x.png256?access_token={}", tileID.z, tileID.x, tileID.y, this->token);
				cpr::Response r;
				cpr::Header cprHeader;
				r = cpr::Get(cpr::Url{ formatUrl },
					cprHeader);
				if (r.status_code == 200)
				{
					if (!r.text.empty())
					{
						auto img = mbgl::decodeImage(r.text);
						size_t size = img.channels * img.size.width * img.size.height;
						std::vector<unsigned char> vec_img(img.data.get(), img.data.get() + size);
						threepp::Image three_img(std::move(vec_img),
							img.size.width, img.size.height);
						std::shared_ptr<threepp::Texture> texture = threepp::Texture::create(std::move(three_img));
						if (img.channels == 4) {
							texture->format = threepp::Format::RGBA;
						}
						else {
							texture->format = threepp::Format::RGB;
						}
						texture->anisotropy = 16;
						texture->wrapS = threepp::TextureWrapping::ClampToEdge;
						texture->wrapT = threepp::TextureWrapping::ClampToEdge;
						texture->minFilter = threepp::Filter::Linear; 
						texture->magFilter = threepp::Filter::Linear; 
						texture->generateMipmaps = true;
						texture->needsUpdate();
						raster_cache->insert(tileID, std::move(texture));
						this->workerManager.releaseWorker(tileID);
					}
				}
				else
				{
					spdlog::error("Error code : {} with message : {}",r.status_code,r.error.message);
				}
			};
			pool.detach_task(f); 
		}
	}
}
