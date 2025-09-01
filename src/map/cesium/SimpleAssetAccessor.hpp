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
class SimpleAssetAccessor : public CesiumAsync::IAssetAccessor {
public:
  SimpleAssetAccessor() {}

  virtual CesiumAsync::Future<std::shared_ptr<CesiumAsync::IAssetRequest>>
  get(const CesiumAsync::AsyncSystem& asyncSystem,
      const std::string& url,
      const std::vector<THeader>&) override
  {
      auto bytes = dragon::StringHelper::readFile(std::filesystem::path(url));
      std::string method = "GET"; 
      CesiumAsync::HttpHeaders requestHeaders;
      std::unique_ptr<SimpleAssetResponse> pResponse;
      pResponse = std::make_unique<SimpleAssetResponse>(
              static_cast<uint16_t>(200),
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

  virtual CesiumAsync::Future<std::shared_ptr<CesiumAsync::IAssetRequest>>
  request(
      const CesiumAsync::AsyncSystem& asyncSystem,
      const std::string& /* verb */,
      const std::string& url,
      const std::vector<THeader>& headers,
      const std::span<const std::byte>&) override {
    return this->get(asyncSystem, url, headers);
  }

  virtual void tick() noexcept override {}

};
} // namespace CesiumNativeTests
