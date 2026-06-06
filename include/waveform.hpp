#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>


// Owning object model of a sound signal loaded into memory.
class Waveform {
public:
    std::uint32_t sampleRate = 0; // number of samples per second, e.g. 44100
    std::uint16_t channelCount = 0; // number of audio channels, e.g. 1 for mono, 2 for stereo
    std::uint16_t bitsPerSample = 0; // number of bits per sample
    std::vector<std::int16_t> samples; // interleaved audio samples

    std::size_t getSampleCount() const;
    double getDurationSeconds() const;
    std::size_t secondsToSamples(double seconds) const;
    std::size_t millisecondsToSamples(double milliseconds) const;
};
