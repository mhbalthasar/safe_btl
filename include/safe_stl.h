//
// Created by balthasar on 2025/4/27.
//

#ifndef XIPC_SAFE_STL_H
#define XIPC_SAFE_STL_H


#include <shared_mutex>
#include <vector>
#include <map>
#include <mutex>
#include "turbo_mutex.h"

namespace btl {
    template<typename Container, typename Mutex>
    class _global_safe_stl {
    private:
        Container obj;              // 被保护对象
        mutable Mutex mtx;     // 互斥锁
    public:
        // 代理对象，持有锁的生命周期
        class Proxy {
        private:
            Container &ref;
            std::unique_lock<Mutex> lock;

        public:
            Proxy(Container &c, Mutex &m) : ref(c), lock(m) {}

            Container *operator->() { return &ref; }  // 重载->操作符
        };

        // 获取代理对象（持有锁）
        Proxy get() { return Proxy(obj, mtx); }
    };


    template<typename Container>
    using safe_stl = _global_safe_stl<Container, std::mutex>;
    template<typename Container>
    using turbo_stl = _global_safe_stl<Container, btl::turbo_mutex>;




    template <typename T>
    class _rwlock_safe_stl {
    private:
        T obj;                      // 封装的STL容器
        mutable std::shared_mutex mtx; // 读写锁

    public:
        // 读操作代理：获取读锁
        class ReadProxy {
        public:
            ReadProxy(const T& obj, std::shared_mutex& mtx)
                    : ref(obj), lock(mtx) {}
            const T* operator->() const { return &ref; }
        private:
            const T& ref;
            std::shared_lock<std::shared_mutex> lock; // 自动管理读锁
        };

        // 写操作代理：获取写锁
        class WriteProxy {
        public:
            WriteProxy(T& obj, std::shared_mutex& mtx)
                    : ref(obj), lock(mtx) {}
            T* operator->() { return &ref; }
            T& operator*() { return ref; }
        private:
            T& ref;
            std::unique_lock<std::shared_mutex> lock; // 自动管理写锁
        };

        // 获取只读接口
        ReadProxy get() const {
            return get_ro();
        }
        ReadProxy get_ro() const {
            return ReadProxy(obj, mtx);
        }

        // 获取可写接口
        WriteProxy get() {
            return get_rw();
        }
        WriteProxy get_rw() {
            return WriteProxy(obj, mtx);
        }
    };

    template<typename Container>
    using shared_stl=_rwlock_safe_stl<Container>;
}
#endif //XIPC_SAFE_STL_H
