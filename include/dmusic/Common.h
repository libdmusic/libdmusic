#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>
#include <sstream>
#include <utility>

/// Use this macro to initialize struct fields which are expected to be loaded as little endian data
#define FIELDINIT(s, f, t) {f = DirectMusic::littleEndianRead<t>(data + offsetof(s, f));}

/// Use this macro to initialize struct fields which are expected to be loaded as little endian data
#define FIELDINITC(s, f, t1, t2) {f = (t2)(DirectMusic::littleEndianRead<t1>(data + offsetof(s, f)));}

/// Use this macro to initialize struct fields which are expected to be loaded as little endian data and need casting
#define FIELDINITE(s, f, t, c) {f = (c) DirectMusic::littleEndianRead<t>(data + offsetof(s, f));}

/// Use this macro to initialize struct fields in constructors that need to load little endian data
#define FIELDCONST(s, f, t) f(t(data + offsetof(s, f)))

/// Use this macro to read a FourCC into a char array
#define READFOURCC(s, f) {memcpy(f, data + offsetof(s, f), 4);}

#if DMUSIC_TRACE
#   define TRACE(x) std::cout << "[libdmusic " << __FILE__ << ":" << __LINE__ << ":" << __func__ << "] " << x << "\n";
#   if DMUSIC_TRACE_VERBOSE
#       define TRACE_VERBOSE(x) std::cout << "[libdmusic " << __FILE__ << ":" << __LINE__ << ":" << __func__ << "] " << x << "\n";
#   else
#       define TRACE_VERBOSE(x)
#   endif
#else
#define TRACE(x)
#define TRACE_VERBOSE(x)
#endif

namespace DirectMusic {
    /**
    * Read an integral type from the given pointer as little endian data
    * @tparam T Integral type
    * @param ptr Pointer to the data to convert
    * @return Integer at *ptr of type T read as little endian
    */
    template<typename T>
    T bigEndianRead(const uint8_t* ptr) {
        const std::size_t numBytes = sizeof(T);
        T result = 0;

        for (std::size_t i = 0; i < numBytes; i++) {
            result |= ptr[i] << (numBytes - i - 1) * 8;
        }

        return result;
    }

    /**
    * Read an integral type from the given pointer as little endian data
    * @tparam T Integral type
    * @param ptr Pointer to the data to convert
    * @return Integer at *ptr of type T read as little endian
    */
    template<typename T>
    T littleEndianRead(const uint8_t* ptr) {
        const std::size_t numBytes = sizeof(T);
        T result = 0;

        for (std::size_t i = 0; i < numBytes; i++) {
            result |= ptr[i] << i * 8;
        }

        return result;
    }

    /**
    * Write an integral type to the given pointer as little endian data
    * @tparam T Integral type
    * @param ptr Pointer to write to
    */
    template<typename T>
    void littleEndianWrite(T val, uint8_t* ptr) {
        const std::size_t numBytes = sizeof(T);

        for (std::size_t i = 0; i < numBytes; i++) {
            ptr[i] = (val & (0xFF << (i * 8))) >> (i * 8);
        }
    }

    /**
    * Write an integral type to the given pointer as big endian data
    * @tparam T Integral type
    * @param ptr Pointer to write to
    */
    template<typename T>
    void bigEndianWrite(T val, uint8_t* ptr) {
        const std::size_t numBytes = sizeof(T);

        for (std::size_t i = 0; i < numBytes; i++) {
            ptr[i] = (val & (0xFF << ((numBytes - i - 1) * 8))) >> ((numBytes - i - 1) * 8);
        }
    }

    /**
     * Converts a buffer containing windows UTF16-Date (wchar_t on windows) to a UTF-8 compatible string
     * @param utf16str Buffer with UTF16-data
     * @param numChars Number of chars to be found in the given buffer
     * @return UTF8 representation of the input string
     */
    inline std::string utf16_to_utf8(const std::uint16_t* utf16str) {
        std::string ret;

        // FIXME: More like utf16 to ascii. Should replace this with something proper, but this is good enough for now.
        for(size_t i = 0; utf16str[i] != '\0'; i++) {
            if(isascii(utf16str[i])) {
                ret += static_cast<char>(utf16str[i]);
            } else {
                ret += u8"\uFFFD"; // Invalid char-sign
            }
        }

        return ret;
    }

#pragma pack(push, 1)
    struct GUID {
        std::uint32_t Data1;
        std::uint16_t Data2;
        std::uint16_t Data3;
        std::uint64_t Data4;

        GUID() {}

        GUID(const std::uint8_t *data) {
            FIELDINIT(GUID, Data1, std::uint32_t);
            FIELDINIT(GUID, Data2, std::uint16_t);
            FIELDINIT(GUID, Data3, std::uint16_t);
            FIELDINIT(GUID, Data4, std::uint64_t);
        }

        std::string toString() const {
            std::stringstream str;
            str << std::hex << "{" << Data1 << "-" << Data2 << "-" << Data3 << "-" << Data4 << "}";
            return str.str();
        }
    };
#pragma pack(pop)

    inline bool operator<(const GUID& lhs, const GUID& rhs) {
        return memcmp(&lhs, &rhs, sizeof(rhs)) < 0;
    }

    inline bool operator==(const GUID& lhs, const GUID& rhs) {
        return memcmp(&lhs, &rhs, sizeof(rhs)) == 0;
    }

    inline GUID operator^(const GUID& lhs, const GUID& rhs) {
        GUID ret;
        ret.Data1 = lhs.Data1 ^ rhs.Data1;
        ret.Data2 = lhs.Data2 ^ rhs.Data2;
        ret.Data3 = lhs.Data3 ^ rhs.Data3;
        ret.Data4 = lhs.Data4 ^ rhs.Data4;
        return ret;
    }
}

namespace std {
    template<> struct hash<DirectMusic::GUID> {
        typedef DirectMusic::GUID argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept {
            result_type const h1(std::hash<std::uint32_t>{}(s.Data1));
            result_type const h2(std::hash<std::uint16_t>{}(s.Data2));
            result_type const h3(std::hash<std::uint16_t>{}(s.Data3));
            result_type const h4(std::hash<std::uint64_t>{}(s.Data4));
            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

    template<typename T1, typename T2> struct hash<std::pair<T1,T2>> {
        typedef std::pair<T1, T2> argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept {
            result_type const h1(std::hash<T1>{}(s.first));
            result_type const h2(std::hash<T2>{}(s.second));
            return h1 ^ (h2 << 1);
        }
    };
}