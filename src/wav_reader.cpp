#include "wav_reader.hpp"
#include "wav_format.hpp"

#include <cstring>
#include <fstream>

namespace {

bool hasId(const char actual[4], const char expected[4])
{
    return std::memcmp(actual, expected, 4) == 0;
}

}  // namespace

// parse WAV file and store the result in waveform
WavReader::Result WavReader::read(const std::string& fileName,
                                  Waveform& waveform)
{
    waveform = {};
    _errorMessage.clear();

    std::ifstream input(fileName, std::ios::binary);
    if(!input)
    {
        _errorMessage = "Cannot open WAV file: " + fileName;
        return Result::CannotOpenFile;
    }

    Result result = readRiffHeader(input);
    if(result != Result::Success)
        return result;

    WavFileInfo fileInfo;
    result = findChunks(input, waveform, fileInfo);
    if(result != Result::Success)
        return result;

    result = validateFormat(waveform, fileInfo);
    if(result != Result::Success)
        return result;

    return readSamples(input, waveform, fileInfo);
}

// check that the file starts with a valid RIFF/WAVE header
WavReader::Result WavReader::readRiffHeader(std::istream& input)
{
    wav::RiffHeader header = {};
    if(!wav::readObject(input, header) || !hasId(header.sign, "RIFF") ||
       !hasId(header.waveId, "WAVE"))
    {
        _errorMessage = "The file is not a valid RIFF/WAVE file";
        return Result::InvalidFile;
    }

    return Result::Success;
}

WavReader::Result WavReader::findChunks(std::istream& input, Waveform& waveform,
                                        WavFileInfo& fileInfo)
{
    // parse fmt chunk and data chunk in Waveform and WavFileInfo while we dont
    // found them and the file is not ended
    while(input && (!fileInfo.formatFound || !fileInfo.dataFound))
    {
        wav::ChunkHeader chunkHeader = {};
        if(!wav::readObject(input, chunkHeader))
            break;

        const std::streampos chunkDataPosition =
            input.tellg();  // save the position of chunk data
        if(hasId(chunkHeader.id, "fmt "))
        {
            wav::FmtChunkData fmtData = {};
            if(chunkHeader.size < sizeof(wav::FmtChunkData) ||
               !wav::readObject(input, fmtData))
            {
                _errorMessage = "Invalid WAV format chunk";
                return Result::InvalidFile;
            }

            fileInfo.audioFormat = fmtData.audioFormat;
            fileInfo.blockAlign = fmtData.blockAlign;
            waveform.channelCount = fmtData.channelCount;
            waveform.sampleRate = fmtData.sampleRate;
            waveform.bitsPerSample = fmtData.bitsPerSample;
            fileInfo.formatFound = true;
        }
        else if(hasId(chunkHeader.id, "data"))
        {
            fileInfo.dataPosition = chunkDataPosition;
            fileInfo.dataSize = chunkHeader.size;
            fileInfo.dataFound = true;
        }

        const std::streamoff paddedSize =
            static_cast<std::streamoff>(chunkHeader.size) +
            static_cast<std::streamoff>(
                chunkHeader.size %
                2);  // calculate padded size of the next chunk data
        input.seekg(chunkDataPosition +
                    paddedSize);  // jump to the next chunk header
    }

    if(!fileInfo.formatFound ||
       !fileInfo.dataFound)  // if we dont found fmt or data chunk
    {
        _errorMessage = "WAV file must contain fmt and data chunks";
        return Result::InvalidFile;
    }

    return Result::Success;
}

WavReader::Result WavReader::validateFormat(const Waveform& waveform,
                                            const WavFileInfo& fileInfo)
{
    if(fileInfo.audioFormat != 1)
    {
        _errorMessage = "Only uncompressed PCM WAV files are supported";
        return Result::UnsupportedFormat;
    }
    if(waveform.channelCount != 1)
    {
        _errorMessage = "Only mono WAV files are supported";
        return Result::UnsupportedFormat;
    }
    if(waveform.bitsPerSample != 16)
    {
        _errorMessage = "Only 16-bit WAV files are supported";
        return Result::UnsupportedFormat;
    }
    if(waveform.sampleRate != 44100)
    {
        _errorMessage = "Only 44100 Hz WAV files are supported";
        return Result::UnsupportedFormat;
    }
    if(waveform.channelCount == 0 || waveform.sampleRate == 0 ||
       fileInfo.blockAlign != waveform.channelCount * sizeof(std::int16_t) ||
       fileInfo.dataSize % fileInfo.blockAlign != 0)
    {
        _errorMessage = "Invalid WAV audio parameters";
        return Result::InvalidFile;
    }

    return Result::Success;
}

// fill the waveform samples vector with data from the file
WavReader::Result WavReader::readSamples(std::istream& input,
                                         Waveform& waveform,
                                         const WavFileInfo& fileInfo)
{
    waveform.samples.resize(fileInfo.dataSize / sizeof(std::int16_t));
    input.clear();
    input.seekg(fileInfo.dataPosition);
    if(fileInfo.dataSize != 0 &&
       !input.read(reinterpret_cast<char*>(waveform.samples.data()),
                   static_cast<std::streamsize>(fileInfo.dataSize)))
    {
        _errorMessage = "WAV sample data is incomplete";
        return Result::InvalidFile;
    }

    return Result::Success;
}

const std::string& WavReader::getErrorMessage() const { return _errorMessage; }
