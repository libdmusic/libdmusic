#include <cstdint>
#include <vector>
#include <dmusic/PlayingContext.h>
#include <dmusic/Riff.h>

using namespace DirectMusic;
using namespace DirectMusic::DLS;
using namespace DirectMusic::Riff;

template<typename T>
static std::shared_ptr<T> genObjFromChunkData(const std::vector<std::uint8_t>& data) {
    if (data.empty()) return nullptr;
    Chunk c(data.data());
    return std::make_shared<T>(c);
}

template<typename T>
PlayingContext<T>::PlayingContext(std::uint32_t sampleRate, std::uint32_t audioChannels)
    : m_sampleRate(sampleRate),
    m_audioChannels(audioChannels),
    m_loader(Loader())
{}

template<typename T>
std::shared_ptr<DownloadableSound> PlayingContext<T>::loadInstrumentCollection(const std::string& name) const {
    std::vector<std::uint8_t> data = m_loader.loadFile(name);
    return genObjFromChunkData<DownloadableSound>(data);
}

template<typename T>
std::shared_ptr<SegmentForm> PlayingContext<T>::loadSegment(const std::string& name) const {
    std::vector<std::uint8_t> data = m_loader.loadFile(name);
    return genObjFromChunkData<SegmentForm>(data);
}

template<typename T>
std::shared_ptr<StyleForm> PlayingContext<T>::loadStyle(const std::string& name) const {
    std::vector<std::uint8_t> data = m_loader.loadFile(name);
    return genObjFromChunkData<StyleForm>(data);
}
