#pragma once
#include <MinHook.h>
#include <cocos2d.h>
#include <random>

namespace utils
{
#define MEMBERBYOFFSET(type, class, offset) *reinterpret_cast<type*>(reinterpret_cast<uintptr_t>(class) + offset)
#define MBO MEMBERBYOFFSET

    inline uintptr_t gd_base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
    inline uintptr_t cc_base = reinterpret_cast<uintptr_t>(GetModuleHandle("libcocos2d.dll"));
    inline uintptr_t fmod_base = reinterpret_cast<uintptr_t>(GetModuleHandle("fmod.dll"));

    inline std::vector<uint8_t> hexToBytes(const std::string& hex)
    {
        std::vector<uint8_t> bytes;

        for (unsigned int i = 0; i < hex.length(); i += 3)
        {
            std::string byteString = hex.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
            bytes.push_back(byte);
        }

        return bytes;
    }

    inline bool writeBytes(std::uintptr_t const address, std::vector<uint8_t> const& bytes)
    {
        return WriteProcessMemory(
            GetCurrentProcess(),
            reinterpret_cast<LPVOID>(address),
            bytes.data(), bytes.size(),
            nullptr
        );
    }

    inline std::vector<std::string> split(std::string s, std::string delimiter)
    {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::string token;
        std::vector<std::string> res;

        while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
        {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }

    inline int randomInt(int min, int max)
    {
        std::random_device device;
        std::mt19937 generator(device());
        std::uniform_int_distribution<int> distribution(min, max);

        return distribution(generator);
    }
}