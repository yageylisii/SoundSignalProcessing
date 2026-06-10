#include "wav/wav_reader.hpp"
#include "wav/wav_format.hpp"

#include <cstring>
#include <fstream>

// compare 4-character chunk IDs in WAV file headers
static bool hasId(const char actual[4], const char expected[4])
{
    return std::memcmp(actual, expected, 4) == 0;
}

// parse WAV file and store the result in waveform
WavReader::Result WavReader::read(const std::string& fileName,
                                  Waveform& waveform)
{
    waveform = {};
    _errorMessage.clear();

    std::ifstream input(
        fileName,
        std::ios::binary);  // create input file strem for readin WAV file
    if(!input)
    {
        _errorMessage = "Cannot open WAV file: " + fileName;
        return Result::CannotOpenFile;
    }

    Result result = readRiffHeader(input);  // read the header
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

// check the file is valid
WavReader::Result WavReader::readRiffHeader(std::istream& input)
{
    RiffHeader header = {};  // empty structure
    if(!readObject(input, header) || !hasId(header.sign, "RIFF") ||
       !hasId(header.waveId, "WAVE"))
    {
        _errorMessage = "The file is not a valid RIFF/WAVE file";
        return Result::InvalidFile;
    }

    return Result::Success;
}

// parse fmt chunk and data chunk in Waveform and WavFileInfo while we dont
WavReader::Result WavReader::findChunks(std::istream& input, Waveform& waveform,
                                        WavFileInfo& fileInfo)
{
    // found them and the file is not ended
    while(input && (!fileInfo.formatFound || !fileInfo.dataFound))
    {
        ChunkHeader chunkHeader = {};
        if(!readObject(input, chunkHeader))
            break;

        const std::streampos chunkDataPosition =
            input.tellg();  // save the position of chunk data
        if(hasId(chunkHeader.id, "fmt "))
        {
            FmtChunkData fmtData = {};
            if(chunkHeader.size < sizeof(FmtChunkData) ||
               !readObject(input, fmtData))
            {
                _errorMessage = "Invalid WAV format chunk";
                return Result::InvalidFile;
            }

            fileInfo.audioFormat = fmtData.audioFormat;
            fileInfo.blockAlign = fmtData.blockAlign;
            fileInfo.byteRate = fmtData.byteRate;
            waveform.setChannelCount(fmtData.channelCount);
            waveform.setSampleRate(fmtData.sampleRate);
            waveform.setBitsPerSample(fmtData.bitsPerSample);
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
       !fileInfo.dataFound)  // if we did not found fmt or data chunk
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
    if(waveform.getChannelCount() != 1)
    {
        _errorMessage = "Only mono WAV files are supported";
        return Result::UnsupportedFormat;
    }
    if(waveform.getBitsPerSample() != 16)
    {
        _errorMessage = "Only 16-bit WAV files are supported";
        return Result::UnsupportedFormat;
    }
    if(waveform.getSampleRate() != 44100)
    {
        _errorMessage = "Only 44100 Hz WAV files are supported";
        return Result::UnsupportedFormat;
    }
    if(waveform.getChannelCount() == 0 || waveform.getSampleRate() == 0 ||
       fileInfo.blockAlign !=
           waveform.getChannelCount() * sizeof(std::int16_t) ||
       fileInfo.byteRate != waveform.getSampleRate() * fileInfo.blockAlign ||
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
    auto& samples = waveform.getSamples();
    samples.resize(fileInfo.dataSize /
                   sizeof(std::int16_t));  // get memory for samples vector
    input.clear();
    input.seekg(fileInfo.dataPosition);
    if(fileInfo.dataSize != 0 &&
       !input.read(reinterpret_cast<char*>(samples.data()),
                   static_cast<std::streamsize>(fileInfo.dataSize)))
    {
        _errorMessage = "WAV sample data is incomplete";
        return Result::InvalidFile;
    }

    return Result::Success;
}

const std::string& WavReader::getErrorMessage() const { return _errorMessage; }
