#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// Owning object model of a sound signal loaded from file into Waveform class
class Waveform
{
private:
    std::uint32_t _sampleRate = 0;
    std::uint16_t _channelCount = 0;
    std::uint16_t _bitsPerSample = 0;
    std::vector<std::int16_t> _samples;

public:
    std::uint32_t getSampleRate() const;
    std::uint16_t getChannelCount() const;
    std::uint16_t getBitsPerSample() const;
    const std::vector<std::int16_t>& getSamples() const;
    std::vector<std::int16_t>& getSamples();

    void setSampleRate(std::uint32_t sampleRate);
    void setChannelCount(std::uint16_t channelCount);
    void setBitsPerSample(std::uint16_t bitsPerSample);
    void setSamples(std::vector<std::int16_t> samples);

    std::size_t getSampleCount() const;
    double getDurationSeconds() const;
    std::size_t secondsToSamples(double seconds) const;
    std::size_t millisecondsToSamples(double milliseconds) const;
};
