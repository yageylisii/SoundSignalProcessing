#pragma once

#include "waveform.hpp"

#include <cstdint>
#include <iosfwd>
#include <string>

// read WAV file and store the result in a Waveform
class WavReader {
public:
    enum class Result {
        Success,
        CannotOpenFile,
        InvalidFile,
        UnsupportedFormat
    };

    Result read(const std::string& fileName, Waveform& waveform);

    const std::string& getErrorMessage() const;

private:
    struct WavFileInfo {
        std::uint16_t audioFormat = 0;
        std::uint16_t blockAlign = 0;
        std::streampos dataPosition = {};
        std::uint32_t dataSize = 0;
        bool formatFound = false;
        bool dataFound = false;
    };

    Result readRiffHeader(std::istream& input);
    Result findChunks(
        std::istream& input,
        Waveform& waveform,
        WavFileInfo& fileInfo);
    Result validateFormat(
        const Waveform& waveform,
        const WavFileInfo& fileInfo);
    Result readSamples(
        std::istream& input,
        Waveform& waveform,
        const WavFileInfo& fileInfo);

    std::string _errorMessage;
};
