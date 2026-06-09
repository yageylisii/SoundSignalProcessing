#pragma once

#include "filters/common/filter.hpp"
#include "parser/console_parser.hpp"
#include "pipeline/pipeline.hpp"

#include <map>
#include <memory>
#include <string>
#include <vector>

// smart pointer to a function that creates a filter by its descriptor
using FilterProducer =
    std::unique_ptr<IFilter> (*)(const FilterDescriptor& descriptor);

class CmdLineArgs2PipelineConverter
{
private:
    std::unique_ptr<IFilter>
    createFilter(const FilterDescriptor& descriptor) const;

    std::map<std::string, FilterProducer> _producers;

public:
    CmdLineArgs2PipelineConverter();

    Pipeline
    createPipeline(const std::vector<FilterDescriptor>& descriptors) const;

    void addFilterProducer(const std::string& filterName,
                           FilterProducer producer);
    FilterProducer getFilterProducer(const std::string& filterName) const;
};
