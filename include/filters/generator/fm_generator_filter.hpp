#pragma once

#include "filters/common/filter.hpp"


// Replaces the input signal with a frequency-modulated sine wave.
class FmGeneratorFilter final : public IFilter {
public:
    FmGeneratorFilter(
        double amplitude,
        double carrierHz,
        double modulationHz,
        double deviationHz,
        double durationMs);

    FilterState apply(Waveform* sound) override;

private:
    double _amplitude;
    double _carrierHz;
    double _modulationHz;
    double _deviationHz;
    double _durationMs;
};
