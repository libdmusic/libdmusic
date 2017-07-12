#pragma once

#include <string>
#include <cstdint>
#include <vector>

namespace DirectMusic {
    /// Provides a way for applications to customize the way file references are
    /// resolved when loading segments, styles and instrument collections
    class Loader {
    public:
        std::vector<std::uint8_t> loadFile(const std::string& name) const;
    };
}
