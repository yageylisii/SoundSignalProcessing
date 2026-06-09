#include "pipeline/pipeline.hpp"

#include <stdexcept>
#include <utility>

// apply filters to the sound and return the result of the last filter
FilterState Pipeline::apply(Waveform* sound)
{
    _failedFilterIndex = _filters.size();

    for (std::size_t index = 0; index < _filters.size(); ++index) {
        const FilterState state = _filters[index]->apply(sound);
        if (state != FilterState::Success) {
            _failedFilterIndex = index;
            return state;
        }
    }

    return FilterState::Success;
}

IFilter* Pipeline::addFilter(std::unique_ptr<IFilter> filter)
{
    if (!filter) {
        return nullptr;
    }

    IFilter* addedFilter = filter.get();
    _filters.push_back(std::move(filter));
    return addedFilter;
}

std::size_t Pipeline::getFilterCount() const
{
    return _filters.size();
}

std::size_t Pipeline::getFailedFilterIndex() const
{
    return _failedFilterIndex;
}

// return filter by index
IFilter* Pipeline::operator[](std::size_t index)
{
    if (index >= _filters.size()) {
        throw std::out_of_range("Pipeline filter index is out of range");
    }
    return _filters[index].get();
}

const IFilter* Pipeline::operator[](std::size_t index) const
{
    if (index >= _filters.size()) {
        throw std::out_of_range("Pipeline filter index is out of range");
    }
    return _filters[index].get();
}
