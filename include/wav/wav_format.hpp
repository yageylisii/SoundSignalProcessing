#pragma once

#include <cstdint>
#include <istream>
#include <ostream>


#pragma pack(push, 1)

// first 12 bytes of a WAV file
struct RiffHeader {
    char sign[4]; // should be "RIFF"
    std::uint32_t size; // size of the entire file minus 8 bytes for sign and size
    char waveId[4]; // should be "WAVE"
};

// header of each chunk in a WAV file
struct ChunkHeader {
    char id[4]; // name of the chunk, "fmt " or "data"
    std::uint32_t size; // size of the chunk data in bytes, excluding padding
};

// data of the "fmt " chunk in a WAV file
struct FmtChunkData {
    std::uint16_t audioFormat; // should be 1 for uncompressed PCM
    std::uint16_t channelCount; // number of audio channels
    std::uint32_t sampleRate; // sample rate in Hz
    std::uint32_t byteRate; // byte rate = sampleRate * channelCount * bitsPerSample / 8
    std::uint16_t blockAlign; //! block alignment = channelCount * bitsPerSample / 8
    std::uint16_t bitsPerSample; //! number of bits per sample
};

#pragma pack(pop)

static_assert(sizeof(RiffHeader) == 12);
static_assert(sizeof(ChunkHeader) == 8);
static_assert(sizeof(FmtChunkData) == 16);


// read sizeof(object) bytes from the input stream into object
template <typename T>
bool readObject(std::istream& input, T& object)
{
    return static_cast<bool>(
        input.read(reinterpret_cast<char*>(&object), sizeof(object)));
}
// write an object of type T to a binary stream
template <typename T>
bool writeObject(std::ostream& output, const T& object)
{
    return static_cast<bool>(
        output.write(reinterpret_cast<const char*>(&object), sizeof(object)));
}
