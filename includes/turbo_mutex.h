//
// Created by balthasar on 2025/4/27.
//
#pragma once
#ifndef XIPC_TURBO_MUTEX_H
#define XIPC_TURBO_MUTEX_H

#if defined(WIN32)
#include <windows.h>
#endif
#include <mutex>

namespace btl{
    class _windows_turbo_mutex{
    public:
        explicit _windows_turbo_mutex() noexcept{
            //InitializeCriticalSection(&socket_mutex);
            DWORD dwSpin=4000;
            InitializeCriticalSectionAndSpinCount(&socket_mutex,dwSpin);
            SetCriticalSectionSpinCount(&socket_mutex,(dwSpin & ~0x80000000));//非递归锁
        }
        virtual ~_windows_turbo_mutex() noexcept{
            DeleteCriticalSection(&socket_mutex);
        }
        void lock(){
            EnterCriticalSection(&socket_mutex);
        }
        void unlock(){
            LeaveCriticalSection(&socket_mutex);
        }

        bool try_lock(){
            return (TryEnterCriticalSection(&socket_mutex)!=0);
        }

        _windows_turbo_mutex(const _windows_turbo_mutex&) = delete;
        _windows_turbo_mutex& operator=(const _windows_turbo_mutex&) =delete;

    private:
        CRITICAL_SECTION socket_mutex{};
    };

#ifdef WIN32
    using turbo_mutex=_windows_turbo_mutex;
#else
    using turbo_mutex=std::mutex;
#endif
}
#endif //XIPC_TURBO_MUTEX_H
