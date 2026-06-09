#pragma once

#include "filters/common/filter.hpp"


// Inserts zero samples for the silence time interval. 
// The recording time is increased
class SilenceFilter final : public IFilter {
public:
    SilenceFilter(TimeUnit unit, double start, double end);

    FilterState apply(Waveform* sound) override;

    TimeUnit getTimeUnit() const;
    double getStart() const;
    double getEnd() const;

private:
    TimeUnit _unit;
    double _start;
    double _end;
};
