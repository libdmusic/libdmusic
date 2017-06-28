#pragma once

#include <cstdint>

namespace DirectMusic {
    struct GUID {
        std::uint64_t Data1;
        std::uint32_t Data2;
        std::uint32_t Data3;
        std::uint64_t Data4;
    };
}
