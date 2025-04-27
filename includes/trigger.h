//
// Created by balthasar on 2025/4/27.
//
#pragma once
#ifndef XIPC_TRIGGER_H
#define XIPC_TRIGGER_H

#include <mutex>
#include <condition_variable>
#include <chrono>
#include "turbo_mutex.h"

namespace btl{
    template<typename _mutex>
    class _trigger {
    public:
        // 无限等待直到触发（返回true）
        bool wait() {
            std::unique_lock<_mutex> lock(mtx_);
            cv_.wait(lock, [this]{ return triggered_; });
            triggered_ = false; // 重置触发状态
            return true;
        }

        // 带超时等待（返回是否触发）
        bool wait(uint32_t timeout_ms) {
            std::unique_lock<_mutex> lock(mtx_);
            bool ret = cv_.wait_for(lock,
                                    std::chrono::milliseconds(timeout_ms),
                                    [this]{ return triggered_; });

            if(ret) triggered_ = false; // 仅触发时重置状态
            return ret;
        }

        // 触发单个等待线程
        void notify_one() noexcept {
            std::lock_guard<_mutex> lock(mtx_);
            triggered_ = true;
            cv_.notify_one();
        }

        // 触发所有等待线程
        void notify_all() noexcept {
            std::lock_guard<_mutex> lock(mtx_);
            triggered_ = true;
            cv_.notify_all();
        }

    private:
        bool triggered_ = false;          // 触发状态标志[1,3](@ref)
        _mutex mtx_;                  // 互斥锁[4](@ref)
        std::condition_variable cv_;      // 条件变量[6](@ref)
    };

    typedef _trigger<std::mutex> trigger;
    typedef _trigger<btl::turbo_mutex> turbo_trigger;
}

#endif //XIPC_TRIGGER_H
