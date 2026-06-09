#pragma once

#include "wav/waveform.hpp"


enum class FilterState {
    Success,
    InvalidWaveform,
    EmptyWaveform,
    InvalidParameter
};

enum class TimeUnit {
    Seconds,
    Milliseconds
};

class IFilter {
public:
    // five^s rule
    IFilter() = default;
    IFilter(const IFilter&) = default;
    IFilter(IFilter&&) noexcept = default;
    IFilter& operator=(const IFilter&) = default;
    IFilter& operator=(IFilter&&) noexcept = default;
    virtual ~IFilter() = default;

    virtual FilterState apply(Waveform* sound) = 0;
};
