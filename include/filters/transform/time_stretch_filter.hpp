#pragma once

#include "filters/common/filter.hpp"


// Changes signal duration using linear interpolation between source samples.
class TimeStretchFilter final : public IFilter {
public:
    explicit TimeStretchFilter(double factor);

    FilterState apply(Waveform* sound) override;

    double getFactor() const;
    void setFactor(double factor);

private:
    double _factor;
};
