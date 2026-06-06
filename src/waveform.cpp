#include "waveform.hpp"

#include <cmath>
#include <limits>


std::size_t Waveform::getSampleCount() const
{
    return samples.size();
}

double Waveform::getDurationSeconds() const
{
    if (sampleRate == 0 || channelCount == 0) {
        return 0.0;
    }

    return static_cast<double>(samples.size())
         / static_cast<double>(sampleRate * channelCount);
}

std::size_t Waveform::secondsToSamples(double seconds) const
{
    if (seconds <= 0.0 || sampleRate == 0 || channelCount == 0) {
        return 0;
    }

    const double sampleCount = seconds * sampleRate * channelCount;
    if (sampleCount >= std::numeric_limits<std::size_t>::max()) {
        return std::numeric_limits<std::size_t>::max();
    }
    return static_cast<std::size_t>(std::llround(sampleCount));
}

std::size_t Waveform::millisecondsToSamples(double milliseconds) const
{
    return secondsToSamples(milliseconds / 1000.0);
}
