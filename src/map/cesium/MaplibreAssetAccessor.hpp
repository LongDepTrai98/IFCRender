#pragma once
#include <CesiumAsync/AsyncSystem.h>
#include <CesiumAsync/IAssetAccessor.h>
#include <CesiumAsync/IAssetRequest.h>
#include "SimpleAssetRequest.hpp"
#include "SimpleAssetResponse.hpp"
#include <core/utils/StringHelper.hpp>
#include <cstddef>
#include <map>
#include <memory>
#include <filesystem>




namespace CesiumNativeTests {
class MaplibreAssetAccessor : public CesiumAsync::IAssetAccessor {
public:
    MaplibreAssetAccessor(); 

    virtual CesiumAsync::Future<std::shared_ptr<CesiumAsync::IAssetRequest>> get(const CesiumAsync::AsyncSystem& asyncSystem,
            const std::string& url,
            const std::vector<THeader>& header) override; 

    virtual CesiumAsync::Future<std::shared_ptr<CesiumAsync::IAssetRequest>> request(
            const CesiumAsync::AsyncSystem& asyncSystem,
            const std::string& /* verb */,
            const std::string& url,
            const std::vector<THeader>& headers,
            const std::span<const std::byte>&) override; 
    CesiumAsync::Future<std::shared_ptr<CesiumAsync::IAssetRequest>> process_request(std::string method, const CesiumAsync::AsyncSystem& asyncSystem, const std::string& url, const std::vector<THeader>& headers /*= {}*/);

  virtual void tick() noexcept override; 

};
} // namespace CesiumNativeTests
