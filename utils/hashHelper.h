#pragma once
#include <xhash>
namespace Details 
{
    void hash_combine(size_t& seed, size_t hash)
    {
        hash += 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hash;
    }
}

namespace HashSpace
{
    template<typename T>
    void hash_combine(size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        Details::hash_combine(seed, hasher(v));
    }

    template<typename T>
    void hash_param(size_t& seed, const T& t)
    {
        hash_combine(seed, t);
    }

    template<typename T, typename... Args>
    void hash_param(size_t &seed, const T& first, const Args &... args)
    {
        hash_param(seed, first);

        hash_param(seed, args...);
    }
}
namespace Log
{
    template<typename T>
    void selfPrint(const T& t)
    {
        std::cout << t << std::endl;
    }

    template<typename T, typename ...Args>
    void selfPrint(const T& t, const Args& ... args)
    {
        selfPrint(t);
        selfPrint(args...);
    }
}