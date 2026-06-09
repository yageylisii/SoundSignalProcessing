#include "wav/waveform.hpp"

#include <cmath>
#include <limits>
#include <utility>


std::uint32_t Waveform::getSampleRate() const
{
    return _sampleRate;
}

std::uint16_t Waveform::getChannelCount() const
{
    return _channelCount;
}

std::uint16_t Waveform::getBitsPerSample() const
{
    return _bitsPerSample;
}

const std::vector<std::int16_t>& Waveform::getSamples() const
{
    return _samples;
}

std::vector<std::int16_t>& Waveform::getSamples()
{
    return _samples;
}

void Waveform::setSampleRate(std::uint32_t sampleRate)
{
    _sampleRate = sampleRate;
}

void Waveform::setChannelCount(std::uint16_t channelCount)
{
    _channelCount = channelCount;
}

void Waveform::setBitsPerSample(std::uint16_t bitsPerSample)
{
    _bitsPerSample = bitsPerSample;
}

void Waveform::setSamples(std::vector<std::int16_t> samples)
{
    _samples = std::move(samples);
}

std::size_t Waveform::getSampleCount() const
{
    return _samples.size();
}

double Waveform::getDurationSeconds() const // calculate duration of the sound in seconds
{
    if (_sampleRate == 0 || _channelCount == 0) {
        return 0.0;
    }

    return static_cast<double>(_samples.size())
         / static_cast<double>(_sampleRate * _channelCount);
}

std::size_t Waveform::secondsToSamples(double seconds) const // transform seconds to count of samples
{
    if (seconds <= 0.0 || _sampleRate == 0 || _channelCount == 0) {
        return 0;
    }

    const double sampleCount = seconds * _sampleRate * _channelCount;
    if (sampleCount >= std::numeric_limits<std::size_t>::max()) {
        return std::numeric_limits<std::size_t>::max();
    }
    return static_cast<std::size_t>(std::llround(sampleCount));
}

std::size_t Waveform::millisecondsToSamples(double milliseconds) const // transform milliseconds to count of samples
{
    return secondsToSamples(milliseconds / 1000.0);
}
