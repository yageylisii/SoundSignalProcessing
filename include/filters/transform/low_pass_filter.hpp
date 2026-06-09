#pragma once

#include "filters/common/filter.hpp"

#include <cstddef>


// Smooths the signal by averaging samples in a fixed-size odd window.
class LowPassFilter final : public IFilter {
public:
    explicit LowPassFilter(std::size_t windowSize);

    FilterState apply(Waveform* sound) override;

    std::size_t getWindowSize() const;
    void setWindowSize(std::size_t windowSize);

private:
    std::size_t _windowSize;
};
