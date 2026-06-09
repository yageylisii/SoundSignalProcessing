#pragma once

#include "filters/common/filter.hpp"

#include <cstddef>
#include <memory>
#include <vector>


class Pipeline {
public:
    Pipeline() = default;
    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;
    Pipeline(Pipeline&&) noexcept = default;
    Pipeline& operator=(Pipeline&&) noexcept = default;
    ~Pipeline() = default;

    FilterState apply(Waveform* sound);

    IFilter* addFilter(std::unique_ptr<IFilter> filter);
    std::size_t getFilterCount() const;
    std::size_t getFailedFilterIndex() const;

    IFilter* operator[](std::size_t index);
    const IFilter* operator[](std::size_t index) const;

private:
    std::vector<std::unique_ptr<IFilter>> _filters;
    std::size_t _failedFilterIndex = 0;
};
