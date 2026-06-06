#include "wav_writer.hpp"
#include "wav_format.hpp"

#include <cstdint>
#include <fstream>
#include <limits>


WavWriter::Result WavWriter::write(
    const std::string& fileName,
    const Waveform& waveform)
{
    _errorMessage.clear();

    if (waveform.sampleRate == 0
        || waveform.channelCount == 0
        || waveform.bitsPerSample != 16
        || waveform.samples.size() % waveform.channelCount != 0) {
        _errorMessage = "Cannot write invalid or unsupported audio parameters";
        return Result::UnsupportedFormat;
    }

    const std::uint64_t dataSize64 =
        waveform.samples.size() * sizeof(std::int16_t);
    const std::uint64_t byteRate64 =
        static_cast<std::uint64_t>(waveform.sampleRate)
        * waveform.channelCount
        * sizeof(std::int16_t);
    if (dataSize64 > std::numeric_limits<std::uint32_t>::max() - 36
        || byteRate64 > std::numeric_limits<std::uint32_t>::max()) {
        _errorMessage = "Waveform is too large to write as a WAV file";
        return Result::UnsupportedFormat;
    }

    std::ofstream output(fileName, std::ios::binary);
    if (!output) {
        _errorMessage = "Cannot create WAV file: " + fileName;
        return Result::CannotCreateFile;
    }

    const std::uint32_t dataSize = static_cast<std::uint32_t>(dataSize64);
    const wav::RiffHeader riffHeader = {
        {'R', 'I', 'F', 'F'},
        36 + dataSize,
        {'W', 'A', 'V', 'E'}
    };
    const wav::ChunkHeader fmtHeader = {
        {'f', 'm', 't', ' '},
        sizeof(wav::FmtChunkData)
    };
    const wav::FmtChunkData fmtData = {
        1,
        waveform.channelCount,
        waveform.sampleRate,
        static_cast<std::uint32_t>(byteRate64),
        static_cast<std::uint16_t>(
            waveform.channelCount * sizeof(std::int16_t)),
        waveform.bitsPerSample
    };
    const wav::ChunkHeader dataHeader = {{'d', 'a', 't', 'a'}, dataSize};

    if (!wav::writeObject(output, riffHeader)
        || !wav::writeObject(output, fmtHeader)
        || !wav::writeObject(output, fmtData)
        || !wav::writeObject(output, dataHeader)
        || (dataSize != 0
            && !output.write(
                reinterpret_cast<const char*>(waveform.samples.data()),
                static_cast<std::streamsize>(dataSize)))) {
        _errorMessage = "Failed to write WAV file: " + fileName;
        return Result::WriteError;
    }

    return Result::Success;
}

const std::string& WavWriter::getErrorMessage() const
{
    return _errorMessage;
}
