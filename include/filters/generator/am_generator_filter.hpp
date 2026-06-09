#pragma once

#include "filters/common/filter.hpp"


// Replaces the input signal with an amplitude-modulated sine wave.
class AmGeneratorFilter final : public IFilter {
public:
    AmGeneratorFilter(
        double amplitude,
        double carrierHz,
        double modulationHz,
        double depth,
        double durationMs);

    FilterState apply(Waveform* sound) override;

private:
    double _amplitude;
    double _carrierHz;
    double _modulationHz;
    double _depth;
    double _durationMs;
};
