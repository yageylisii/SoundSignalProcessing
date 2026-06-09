#include "wav/wav_writer.hpp"
#include "wav/wav_format.hpp"

#include <cstdint>
#include <fstream>
#include <limits>


WavWriter::Result WavWriter::write(
    const std::string& fileName,
    const Waveform& waveform)
{
    _errorMessage.clear();

    if (waveform.getSampleRate() != 44100
        || waveform.getChannelCount() != 1
        || waveform.getBitsPerSample() != 16
        || waveform.getSamples().size() % waveform.getChannelCount() != 0) {
        _errorMessage =
            "Only mono 44100 Hz 16-bit PCM audio can be written";
        return Result::UnsupportedFormat;
    }

    const std::uint64_t dataSize64 =
        waveform.getSamples().size() * sizeof(std::int16_t);
    const std::uint64_t blockAlign64 =
        waveform.getChannelCount() * sizeof(std::int16_t);
    const std::uint64_t byteRate64 =
        static_cast<std::uint64_t>(waveform.getSampleRate()) * blockAlign64;

    if (dataSize64 > std::numeric_limits<std::uint32_t>::max() - 36
        || byteRate64 > std::numeric_limits<std::uint32_t>::max()
        || blockAlign64 > std::numeric_limits<std::uint16_t>::max()) {
        _errorMessage = "Waveform is too large to write as a WAV file";
        return Result::UnsupportedFormat;
    }

    std::ofstream output(fileName, std::ios::binary);
    if (!output) {
        _errorMessage = "Cannot create WAV file: " + fileName;
        return Result::CannotCreateFile;
    }

    const std::uint32_t dataSize = static_cast<std::uint32_t>(dataSize64);
    const RiffHeader riffHeader = {
        {'R', 'I', 'F', 'F'},
        36 + dataSize,
        {'W', 'A', 'V', 'E'}
    };
    const ChunkHeader fmtHeader = {
        {'f', 'm', 't', ' '},
        sizeof(FmtChunkData)
    };
    const FmtChunkData fmtData = {
        1,
        waveform.getChannelCount(),
        waveform.getSampleRate(),
        static_cast<std::uint32_t>(byteRate64),
        static_cast<std::uint16_t>(blockAlign64),
        waveform.getBitsPerSample()
    };
    const ChunkHeader dataHeader = {{'d', 'a', 't', 'a'}, dataSize};

    if (!writeObject(output, riffHeader)
        || !writeObject(output, fmtHeader)
        || !writeObject(output, fmtData)
        || !writeObject(output, dataHeader)
        || (dataSize != 0
            && !output.write(
                reinterpret_cast<const char*>(waveform.getSamples().data()),
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
