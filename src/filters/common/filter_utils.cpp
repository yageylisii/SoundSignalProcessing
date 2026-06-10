#include "filter_utils.hpp"

#include <algorithm>
#include <cmath>
#include <limits>


bool isValidWaveform(const Waveform* sound)
{
    return sound != nullptr
        && sound->getSampleRate() != 0
        && sound->getChannelCount() != 0
        && sound->getBitsPerSample() == 16;
}

std::int16_t clampSample(double value)
{
    const double minimum = std::numeric_limits<std::int16_t>::min();
    const double maximum = std::numeric_limits<std::int16_t>::max();
    return static_cast<std::int16_t>(
        std::llround(std::clamp(value, minimum, maximum))); // limited and rounded value
}

bool calculateGeneratorSize(double durationMs, std::size_t& sampleCount)
{
    if (!std::isfinite(durationMs) || durationMs < 0.0) {
        return false;
    }

    const double count = durationMs * GeneratorSampleRate / 1000.0;
    if (count > static_cast<double>(
            std::numeric_limits<std::size_t>::max())) {
        return false;
    }

    sampleCount = static_cast<std::size_t>(std::llround(count));
    return true;
}

void prepareGeneratedWaveform(Waveform& sound, std::size_t sampleCount)
{
    sound.setSampleRate(GeneratorSampleRate);
    sound.setChannelCount(GeneratorChannelCount);
    sound.setBitsPerSample(GeneratorBitsPerSample);
    sound.getSamples().assign(sampleCount, 0); // delete all elements in the vecror and fill it with zeros
}
