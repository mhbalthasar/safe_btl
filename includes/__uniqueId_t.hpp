#pragma clang diagnostic push
#pragma ide diagnostic ignored "bugprone-reserved-identifier"
#pragma once
#ifndef XIPC___UUID_T_HPP
#define XIPC___UUID_T_HPP

#if defined(_WIN32) && !defined(__WINE__)
#include <windows.h>
#include <ctime>
#include <objbase.h>
#else
//Linux头文件
#include <uuid/uuid.h>
//#include <errno.h>
#include <unistd.h>
#include <filesystem>
#include <csignal>
#endif
#include <cstdint>

#if (defined(__GNUC__) || defined(__clang__)) && defined(__SIZEOF_INT128__) && __SIZEOF_INT128__ == 16
#define HAS_UINT128 1
#else
#define HAS_UINT128 0
#endif

#if !HAS_UINT128
    typedef __uint128_t __uniqueId_t;
#else
    namespace btl {
        struct alignas(16) __uuid_int128 {
            uint64_t high; // 高位64位，对应__uint128_t的高地址部分
            uint64_t low;  // 低位64位，对应__uint128_t的低地址部分

            bool operator==(const __uuid_int128 &other) const {
                return low == other.low && high == other.high;
            }

            __uuid_int128 &operator=(const __uuid_int128 &other) {
                if (this != &other) {
                    high = other.high;
                    low = other.low;
                }
                return *this;
            }
        };
    }
    typedef btl::__uuid_int128 __uniqueId_t;

    // 合法特化
    namespace std {
        template<>
        struct hash<btl::__uuid_int128> {
            size_t operator()(const btl::uuid_int128& key) const noexcept{
                return hash<uint64_t>{}(key.high) ^
                       (hash<uint64_t>{}(key.low) << 1);
            }
        };
    }
#endif

namespace btl{
    __uniqueId_t gen_uniqueId(){
    #if defined(WIN32) && !defined(__WINE__)
        UUID uuid;
        char* uuid_str;
        CoCreateGuid(&uuid);
        __uniqueId_t result; //PACK 16字节
        auto bytes = (unsigned char*)&result;
        unsigned char tmemory[16];
        if(sizeof(__uniqueId_t)<16){
            bytes = (unsigned char*)&tmemory;
        }
        bytes[0] = (uuid.Data1 >> 24) & 0xFF;
        bytes[1] = (uuid.Data1 >> 16) & 0xFF;
        bytes[2] = (uuid.Data1 >> 8) & 0xFF;
        bytes[3] = (uuid.Data1) & 0xFF;

        bytes[4] = (uuid.Data2 >> 8 ) & 0xFF;
        bytes[5] = (uuid.Data2) & 0xFF;

        bytes[6] = (uuid.Data3 >> 8 ) & 0xFF;
        bytes[7] = (uuid.Data3) & 0xFF;
        if(sizeof(__uniqueId_t)<16) {
            memcpy(&result,bytes,sizeof(__uniqueId_t));
        }
        return result;
    #else
        uuid_t uuid;
        uuid_generate_random(uuid);
        __uniqueId_t result;
        memcpy(&result, uuid, std::min(sizeof(result),sizeof(uuid)));
        return result;
    #endif
    }
}
#endif //XIPC___UUID_T_HPP

#pragma clang diagnostic pop