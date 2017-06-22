#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace DirectMusic {
    class InvalidChunkException : public std::runtime_error {
    public:
        InvalidChunkException(std::string expectedChunk, std::string actualChunk);
        const std::string getExpectedChunkName() const;
        const std::string getActualChunkName() const;

    private:
        std::string m_expected, m_actual;
    };
}
