#pragma once

#include "filters/common/filter.hpp"


// Scales the signal so its maximum absolute sample reaches the requested peak.
class NormalizeFilter final : public IFilter {
public:
    explicit NormalizeFilter(double peak = 1.0);

    FilterState apply(Waveform* sound) override;

    double getPeak() const;
    void setPeak(double peak);

private:
    double _peak;
};
