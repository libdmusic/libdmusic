#pragma once

#include <cstdint>

namespace DirectMusic {
    #pragma pack(push, 1)
    struct GUID {
        std::uint64_t Data1;
        std::uint32_t Data2;
        std::uint32_t Data3;
        std::uint64_t Data4;
    };
    #pragma pack(pop)
}
