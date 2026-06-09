#include "filters/generator/sin_generator_filter.hpp"

#include "../common/filter_utils.hpp"

#include <cmath>


SinGeneratorFilter::SinGeneratorFilter(
    double frequencyHz,
    double durationMs)
    : _frequencyHz(frequencyHz),
      _durationMs(durationMs)
{
}

FilterState SinGeneratorFilter::apply(Waveform* sound)
{
    if (sound == nullptr) {
        return FilterState::InvalidWaveform;
    }
    std::size_t sampleCount = 0;
    if (!std::isfinite(_frequencyHz) || _frequencyHz < 0.0
        || !calculateGeneratorSize(
            _durationMs,
            sampleCount)) {
        return FilterState::InvalidParameter;
    }

    prepareGeneratedWaveform(*sound, sampleCount);
    for (std::size_t index = 0; index < sampleCount; ++index) {
        const double time =
            static_cast<double>(index) / GeneratorSampleRate;
        sound->getSamples()[index] = clampSample(
            MaximumAmplitude
            * std::sin(2.0 * Pi * _frequencyHz * time));
    }

    return FilterState::Success;
}
