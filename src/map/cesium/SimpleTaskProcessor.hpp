#pragma once

#include <CesiumAsync/ITaskProcessor.h>
#include <spdlog/spdlog.h>
namespace CesiumNativeTests {
class SimpleTaskProcessor : public CesiumAsync::ITaskProcessor {
public:
  virtual void startTask(std::function<void()> f) override {
	  f();
  }
};
} // namespace CesiumNativeTests
