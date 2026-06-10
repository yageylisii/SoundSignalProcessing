#include "filters/transform/normalize_filter.hpp"

#include "../common/filter_utils.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>

NormalizeFilter::NormalizeFilter(double peak): _peak(peak) {}

FilterState NormalizeFilter::apply(Waveform* sound)
{
    if(!isValidWaveform(sound))
        return FilterState::InvalidWaveform;
    if(!std::isfinite(_peak) || _peak < 0.0 || _peak > 1.0)
        return FilterState::InvalidParameter;
    if(sound->getSamples().empty())
        return FilterState::Success;

    std::int32_t currentPeak = 0;
    for(const std::int16_t sample: sound->getSamples())
    {
        const std::int32_t magnitude =
            std::abs(static_cast<std::int32_t>(sample));
        currentPeak = std::max(currentPeak, magnitude);
    }
    if(currentPeak == 0)
        return FilterState::Success;

    const double scale = _peak * MaximumAmplitude / currentPeak;
    for(std::int16_t& sample: sound->getSamples())
        sample = clampSample(sample * scale);

    return FilterState::Success;
}

double NormalizeFilter::getPeak() const { return _peak; }

void NormalizeFilter::setPeak(double peak) { _peak = peak; }
