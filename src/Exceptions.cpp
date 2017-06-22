#include <dmusic/Exceptions.h>
#include <string>

using namespace DirectMusic;

InvalidChunkException::InvalidChunkException(std::string expectedChunk, std::string actualChunk)
    : runtime_error("Invalid chunk name. Expected " + expectedChunk + ", found " + actualChunk),
    m_expected(expectedChunk),
    m_actual(actualChunk)
    {}

const std::string InvalidChunkException::getExpectedChunkName() const {
    return m_expected;
}

const std::string InvalidChunkException::getActualChunkName() const {
    return m_actual;
}
