#pragma once

#include <cstdint>
#include <cstddef>

/// Use this macro to initialize struct fields which are expected to be loaded as little endian data
#define FIELDINIT(s, f, t) f = DirectMusic::littleEndianRead<t>(data + offsetof(s, f))

/// Use this macro to initialize struct fields which are expected to be loaded as little endian data and need casting
#define FIELDINITE(s, f, t, c) f = (c) DirectMusic::littleEndianRead<t>(data + offsetof(s, f))

/// Use this macro to initialize struct fields in constructors that need to load little endian data
#define FIELDCONST(s, f, t) f(t(data + offsetof(s, f)))

/// Use this macro to read a FourCC into a char array
#define READFOURCC(s, f) memcpy(f, data + offsetof(s, f), 4)

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

    struct GUID {
        std::uint64_t Data1;
        std::uint32_t Data2;
        std::uint32_t Data3;
        std::uint64_t Data4;

        GUID() {}

        GUID(const std::uint8_t *data) {
            FIELDINIT(GUID, Data1, std::uint64_t);
            FIELDINIT(GUID, Data2, std::uint32_t);
            FIELDINIT(GUID, Data3, std::uint32_t);
            FIELDINIT(GUID, Data4, std::uint64_t);
        }
    };
}
