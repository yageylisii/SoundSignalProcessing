#pragma once

#include "filters/common/filter.hpp"


// change the volume of the sound by multiplying each sample by the amplification factor
class AmplFilter final : public IFilter {
public:
    explicit AmplFilter(double amplification);
    FilterState apply(Waveform* sound) override;

    double getAmplification() const;
    void setAmplification(double amplification); // amplification factor


private:
    double _amplification;
};
