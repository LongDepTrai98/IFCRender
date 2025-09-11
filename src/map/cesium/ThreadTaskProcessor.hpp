#pragma once

#include <CesiumAsync/ITaskProcessor.h>

#include <thread>
#include <threadpool/BS_thread_pool.hpp>

static BS::thread_pool<BS::tp::none> pool;
namespace CesiumNativeTests {
class ThreadTaskProcessor : public CesiumAsync::ITaskProcessor {
public:
    virtual void startTask(std::function<void()> f) override {
        //std::thread(f).detach();
        pool.detach_task(f);
    }; 
protected: 
    //
};
} // namespace CesiumNativeTests
