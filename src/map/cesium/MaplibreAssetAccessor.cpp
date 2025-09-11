#include "MaplibreAssetAccessor.hpp"
#include <cpr/cpr.h>

namespace CesiumNativeTests {
	MaplibreAssetAccessor::MaplibreAssetAccessor()
	{
	}

	CesiumAsync::Future<std::shared_ptr<CesiumAsync::IAssetRequest>> MaplibreAssetAccessor::get(const CesiumAsync::AsyncSystem& asyncSystem, 
        const std::string& url, 
        const std::vector<THeader>& header)
    {
        cpr::Response r;
        cpr::Header cprHeader; 
        CesiumAsync::HttpHeaders requestHeaders;
        for (auto& h : header)
        {
            cprHeader.insert({ h.first,h.second }); 
            requestHeaders.insert({ h.first,h.second }); 
        }

        std::string method = "GET";
        r = cpr::Get(cpr::Url{ url },
            cprHeader);
        std::vector<std::byte> bytes{};
        if (r.status_code == 200)
        {
            if (!r.text.empty())
            {
                bytes = std::vector<std::byte>(reinterpret_cast<const std::byte*>(r.text.data()),
                    reinterpret_cast<const std::byte*>(r.text.data() + r.text.size()));
            }
        }
        std::unique_ptr<SimpleAssetResponse> pResponse;
        pResponse = std::make_unique<SimpleAssetResponse>(
            static_cast<uint16_t>(r.status_code),
            "doesn't matter",
            CesiumAsync::HttpHeaders{},
            bytes);
        std::shared_ptr<CesiumNativeTests::SimpleAssetRequest> request = std::make_shared<CesiumNativeTests::SimpleAssetRequest>(method,
            url,
            requestHeaders,
            std::move(pResponse));
        return asyncSystem.createResolvedFuture(
            std::shared_ptr<CesiumAsync::IAssetRequest>(std::move(request)));
    }

    CesiumAsync::Future<std::shared_ptr<CesiumAsync::IAssetRequest>> MaplibreAssetAccessor::request(const CesiumAsync::AsyncSystem& asyncSystem, const std::string&, const std::string& url, const std::vector<THeader>& headers, const std::span<const std::byte>&)
    {
        return this->get(asyncSystem, url, headers);
    }

    void MaplibreAssetAccessor::tick() noexcept
    {
    }
}