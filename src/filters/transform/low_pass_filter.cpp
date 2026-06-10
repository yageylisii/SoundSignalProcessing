#include "filters/transform/low_pass_filter.hpp"

#include "../common/filter_utils.hpp"

#include <algorithm>
#include <cstdint>
#include <vector>

LowPassFilter::LowPassFilter(std::size_t windowSize): _windowSize(windowSize) {}

FilterState LowPassFilter::apply(Waveform* sound)
{
    if(!isValidWaveform(sound))
        return FilterState::InvalidWaveform;
    if(sound->getSamples().empty())
        return FilterState::EmptyWaveform;
    if(_windowSize == 0 || _windowSize % 2 == 0)
        return FilterState::InvalidParameter;

    const std::vector<std::int16_t> source = sound->getSamples();
    const std::size_t radius = _windowSize / 2;

    for(std::size_t index = 0; index < source.size(); ++index)
    {
        std::int64_t sum = 0;
        for(std::size_t offset = 0; offset < _windowSize; ++offset)
        {
            const std::int64_t sourceIndex = static_cast<std::int64_t>(index) +
                                             static_cast<std::int64_t>(offset) -
                                             static_cast<std::int64_t>(radius);
            const std::size_t clampedIndex =
                static_cast<std::size_t>(std::clamp<std::int64_t>(
                    sourceIndex, 0,
                    static_cast<std::int64_t>(source.size() - 1)));
            sum += source[clampedIndex];
        }

        sound->getSamples()[index] =
            clampSample(static_cast<double>(sum) / _windowSize);
    }

    return FilterState::Success;
}

std::size_t LowPassFilter::getWindowSize() const { return _windowSize; }

void LowPassFilter::setWindowSize(std::size_t windowSize)
{
    _windowSize = windowSize;
}
