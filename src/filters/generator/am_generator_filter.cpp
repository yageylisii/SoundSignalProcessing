#include "filters/generator/am_generator_filter.hpp"

#include "../common/filter_utils.hpp"

#include <cmath>

AmGeneratorFilter::AmGeneratorFilter(double amplitude, double carrierHz,
                                     double modulationHz, double depth,
                                     double durationMs)
    : _amplitude(amplitude), _carrierHz(carrierHz), _modulationHz(modulationHz),
      _depth(depth), _durationMs(durationMs)
{
}

FilterState AmGeneratorFilter::apply(Waveform* sound)
{
    if(sound == nullptr)
        return FilterState::InvalidWaveform;
    std::size_t sampleCount = 0;
    if(!std::isfinite(_amplitude) || !std::isfinite(_carrierHz) ||
       !std::isfinite(_modulationHz) || !std::isfinite(_depth) ||
       _amplitude < 0.0 || _amplitude > 1.0 || _carrierHz < 0.0 ||
       _modulationHz < 0.0 || _depth < 0.0 || _depth > 1.0 ||
       !calculateGeneratorSize(_durationMs, sampleCount))
    {
        return FilterState::InvalidParameter;
    }

    prepareGeneratedWaveform(*sound, sampleCount);
    for(std::size_t index = 0; index < sampleCount; ++index)
    {
        const double time = static_cast<double>(index) / GeneratorSampleRate;
        const double envelope =
            1.0 + _depth * std::sin(2.0 * Pi * _modulationHz * time);
        const double carrier = std::sin(2.0 * Pi * _carrierHz * time);
        sound->getSamples()[index] =
            clampSample(_amplitude * MaximumAmplitude * envelope * carrier);
    }

    return FilterState::Success;
}
