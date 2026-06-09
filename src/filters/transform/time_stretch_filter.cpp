#include "filters/transform/time_stretch_filter.hpp"

#include "../common/filter_utils.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

TimeStretchFilter::TimeStretchFilter(double factor): _factor(factor) {}

FilterState TimeStretchFilter::apply(Waveform* sound)
{
    if(!isValidWaveform(sound)) // checks
        return FilterState::InvalidWaveform;
    if(sound->getSamples().empty()) // if vector is empty
        return FilterState::EmptyWaveform;
    if(!std::isfinite(_factor) || _factor <= 0.0)
        return FilterState::InvalidParameter;

    const auto& samples = sound->getSamples();
    const double requestedSize = samples.size() * _factor; // new count of samples after stretching
    if(requestedSize > samples.max_size())
        return FilterState::InvalidParameter;

    const std::size_t newSize =
        static_cast<std::size_t>(std::llround(requestedSize));
    std::vector<std::int16_t> stretched(newSize); // create new vector for stretched samples

    for(std::size_t index = 0; index < newSize; ++index) // fill the scretched vector
    {
        const double position = index / _factor; // new position in the original samples vector
        const std::size_t left = std::min(static_cast<std::size_t>(position),
                                          samples.size() - 1);
        const std::size_t right = std::min(left + 1, samples.size() - 1);
        const double fraction = position - left;
        const double value = samples[left] * (1.0 - fraction) +
                             samples[right] * fraction;
        stretched[index] = clampSample(value);
    }

    sound->setSamples(std::move(stretched));
    return FilterState::Success;
}

double TimeStretchFilter::getFactor() const { return _factor; }

void TimeStretchFilter::setFactor(double factor) { _factor = factor; }
