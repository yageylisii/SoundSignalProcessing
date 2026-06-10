#include "filters/transform/ampl_filter.hpp"

#include "../common/filter_utils.hpp"

#include <cmath>

AmplFilter::AmplFilter(double amplification): _amplification(amplification) {}

FilterState AmplFilter::apply(Waveform* sound)
{
    if(!isValidWaveform(sound))
    {  // checks
        return FilterState::InvalidWaveform;
    }
    if(sound->getSamples().empty())
        return FilterState::EmptyWaveform;
    if(!std::isfinite(_amplification) || _amplification < 0.0)
        return FilterState::InvalidParameter;

    for(std::int16_t& sample: sound->getSamples())
        sample = clampSample(sample * _amplification);

    return FilterState::Success;
}

double AmplFilter::getAmplification() const { return _amplification; }

void AmplFilter::setAmplification(double amplification)
{
    _amplification = amplification;
}
