#include "filters/transform/silence_filter.hpp"

#include "../common/filter_utils.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

SilenceFilter::SilenceFilter(TimeUnit unit, double start, double end)
    : _unit(unit), _start(start), _end(end)
{
}

FilterState SilenceFilter::apply(Waveform* sound)
{
    if(!isValidWaveform(sound))
        return FilterState::InvalidWaveform;
    if(sound->getSamples().empty())
        return FilterState::EmptyWaveform;
    if(!std::isfinite(_start) || !std::isfinite(_end) || _start < 0.0 ||
       _end < _start)
    {
        return FilterState::InvalidParameter;
    }
    // start of section
    const std::size_t startSample = _unit == TimeUnit::Seconds
                                        ? sound->secondsToSamples(_start)
                                        : sound->millisecondsToSamples(_start);
    // end of section
    const std::size_t endSample = _unit == TimeUnit::Seconds
                                      ? sound->secondsToSamples(_end)
                                      : sound->millisecondsToSamples(_end);

    if(endSample == std::numeric_limits<std::size_t>::max())
        return FilterState::InvalidParameter;

    const std::size_t silenceSize = endSample - startSample + 1;
    auto& samples = sound->getSamples();
    if(silenceSize > samples.max_size() - samples.size())
        return FilterState::InvalidParameter;

    const std::size_t insertionPosition = std::min(startSample, samples.size());
    samples.insert(samples.begin() + insertionPosition, silenceSize, 0);

    return FilterState::Success;
}

TimeUnit SilenceFilter::getTimeUnit() const { return _unit; }

double SilenceFilter::getStart() const { return _start; }

double SilenceFilter::getEnd() const { return _end; }
