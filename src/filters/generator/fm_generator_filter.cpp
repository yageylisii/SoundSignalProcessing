#include "filters/generator/fm_generator_filter.hpp"

#include "../common/filter_utils.hpp"

#include <cmath>


FmGeneratorFilter::FmGeneratorFilter(
    double amplitude,
    double carrierHz,
    double modulationHz,
    double deviationHz,
    double durationMs)
    : _amplitude(amplitude),
      _carrierHz(carrierHz),
      _modulationHz(modulationHz),
      _deviationHz(deviationHz),
      _durationMs(durationMs)
{
}

FilterState FmGeneratorFilter::apply(Waveform* sound)
{
    if (sound == nullptr) {
        return FilterState::InvalidWaveform;
    }
    std::size_t sampleCount = 0;
    if (!std::isfinite(_amplitude)
        || !std::isfinite(_carrierHz)
        || !std::isfinite(_modulationHz)
        || !std::isfinite(_deviationHz)
        || _amplitude < 0.0 || _amplitude > 1.0
        || _carrierHz < 0.0 || _modulationHz <= 0.0
        || _deviationHz < 0.0
        || !calculateGeneratorSize(
            _durationMs,
            sampleCount)) {
        return FilterState::InvalidParameter;
    }

    prepareGeneratedWaveform(*sound, sampleCount);
    for (std::size_t index = 0; index < sampleCount; ++index) {
        const double time =
            static_cast<double>(index) / GeneratorSampleRate;
        const double phase =
            2.0 * Pi * _carrierHz * time
            + (_deviationHz / _modulationHz)
                * std::sin(
                    2.0 * Pi * _modulationHz * time);
        sound->getSamples()[index] = clampSample(
            _amplitude
            * MaximumAmplitude
            * std::sin(phase));
    }

    return FilterState::Success;
}
