#pragma once

#include "filters/common/filter.hpp"


// Replaces the input signal with a sine wave of the requested frequency.
class SinGeneratorFilter final : public IFilter {
public:
    SinGeneratorFilter(double frequencyHz, double durationMs);

    FilterState apply(Waveform* sound) override;

private:
    double _frequencyHz;
    double _durationMs;
};
