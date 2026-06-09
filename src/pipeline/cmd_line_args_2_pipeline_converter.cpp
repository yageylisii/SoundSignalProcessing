#include "pipeline/cmd_line_args_2_pipeline_converter.hpp"

#include "filters/filters.hpp"

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <utility>

static void requireParameterCount(const FilterDescriptor& descriptor,
                                  std::size_t expected)
{
    if(descriptor.parameters.size() != expected)
    {
        throw std::invalid_argument("Filter '" + descriptor.name +
                                    "' requires " + std::to_string(expected) +
                                    " parameter(s)");
    }
}

static double parseDouble(const FilterDescriptor& descriptor, std::size_t index,
                          const std::string& parameterName)
{
    if(index >= descriptor.parameters.size())
    {
        throw std::invalid_argument("Missing parameter '" + parameterName +
                                    "' for filter '" + descriptor.name + "'");
    }

    const std::string& token = descriptor.parameters[index];
    std::size_t parsedCharacters = 0;
    double value = 0.0;
    try
    {
        value = std::stod(token, &parsedCharacters);
    }
    catch(const std::exception&)
    {
        throw std::invalid_argument("Parameter '" + parameterName +
                                    "' of filter '" + descriptor.name +
                                    "' must be a number");
    }

    if(parsedCharacters != token.size() || !std::isfinite(value))
    {
        throw std::invalid_argument("Parameter '" + parameterName +
                                    "' of filter '" + descriptor.name +
                                    "' must be a finite number");
    }
    return value;
}

static std::size_t parseSize(const FilterDescriptor& descriptor,
                             std::size_t index,
                             const std::string& parameterName)
{
    if(index >= descriptor.parameters.size())
    {
        throw std::invalid_argument("Missing parameter '" + parameterName +
                                    "' for filter '" + descriptor.name + "'");
    }

    const std::string& token = descriptor.parameters[index];
    std::size_t parsedCharacters = 0;
    unsigned long long value = 0;
    try
    {
        value = std::stoull(token, &parsedCharacters);
    }
    catch(const std::exception&)
    {
        throw std::invalid_argument("Parameter '" + parameterName +
                                    "' of filter '" + descriptor.name +
                                    "' must be a positive integer");
    }

    if(parsedCharacters != token.size() ||
       value > std::numeric_limits<std::size_t>::max())
    {
        throw std::invalid_argument("Parameter '" + parameterName +
                                    "' of filter '" + descriptor.name +
                                    "' is outside the valid integer range");
    }
    return static_cast<std::size_t>(value);
}

static std::unique_ptr<IFilter> createAmplFilter(
    const FilterDescriptor& descriptor)
{
    requireParameterCount(descriptor, 1);
    const double factor = parseDouble(descriptor, 0, "factor");
    if(factor < 0.0)
        throw std::invalid_argument("Ampl factor must be non-negative");
    return std::make_unique<AmplFilter>(factor);
}

static std::unique_ptr<IFilter>
createNormalizeFilter(const FilterDescriptor& descriptor)
{
    if(descriptor.parameters.size() > 1)
    {
        throw std::invalid_argument(
            "Filter 'normalize' accepts zero or one parameter");
    }

    const double peak = descriptor.parameters.empty()
                            ? 1.0
                            : parseDouble(descriptor, 0, "peak");
    if(peak < 0.0 || peak > 1.0)
        throw std::invalid_argument("Normalize peak must be between 0 and 1");
    return std::make_unique<NormalizeFilter>(peak);
}

static std::unique_ptr<IFilter> createSilenceFilter(
    const FilterDescriptor& descriptor)
{
    requireParameterCount(descriptor, 3);

    TimeUnit unit;
    if(descriptor.parameters[0] == "sec")
        unit = TimeUnit::Seconds;
    else if(descriptor.parameters[0] == "ms")
        unit = TimeUnit::Milliseconds;
    else
        throw std::invalid_argument("Silence unit must be 'sec' or 'ms'");

    const double start = parseDouble(descriptor, 1, "start");
    const double end = parseDouble(descriptor, 2, "end");
    if(start < 0.0 || end < start)
    {
        throw std::invalid_argument(
            "Silence interval must satisfy 0 <= start <= end");
    }
    return std::make_unique<SilenceFilter>(unit, start, end);
}

static std::unique_ptr<IFilter>
createTimeStretchFilter(const FilterDescriptor& descriptor)
{
    requireParameterCount(descriptor, 1);
    const double factor = parseDouble(descriptor, 0, "factor");
    if(factor <= 0.0)
        throw std::invalid_argument("Timestretch factor must be positive");
    return std::make_unique<TimeStretchFilter>(factor);
}

static std::unique_ptr<IFilter> createLowPassFilter(
    const FilterDescriptor& descriptor)
{
    requireParameterCount(descriptor, 1);
    const std::size_t windowSize = parseSize(descriptor, 0, "window_size");
    if(windowSize == 0 || windowSize % 2 == 0)
    {
        throw std::invalid_argument(
            "Lowpass window_size must be a positive odd integer");
    }
    return std::make_unique<LowPassFilter>(windowSize);
}

static std::unique_ptr<IFilter>
createGeneratorFilter(const FilterDescriptor& descriptor)
{
    if(descriptor.parameters.empty())
    {
        throw std::invalid_argument(
            "Filter 'generator' requires a generator type");
    }

    const std::string& type = descriptor.parameters[0];
    if(type == "sin")
    {
        requireParameterCount(descriptor, 3);
        const double frequency = parseDouble(descriptor, 1, "frequency_hz");
        const double duration = parseDouble(descriptor, 2, "duration_ms");
        if(frequency < 0.0 || duration < 0.0)
        {
            throw std::invalid_argument(
                "Sin generator parameters must be non-negative");
        }
        return std::make_unique<SinGeneratorFilter>(frequency, duration);
    }

    if(type == "am")
    {
        requireParameterCount(descriptor, 6);
        const double amplitude = parseDouble(descriptor, 1, "amplitude");
        const double carrier = parseDouble(descriptor, 2, "carrier_hz");
        const double modulation = parseDouble(descriptor, 3, "modulation_hz");
        const double depth = parseDouble(descriptor, 4, "depth");
        const double duration = parseDouble(descriptor, 5, "duration_ms");
        if(amplitude < 0.0 || amplitude > 1.0 || carrier < 0.0 ||
           modulation < 0.0 || depth < 0.0 || depth > 1.0 || duration < 0.0)
        {
            throw std::invalid_argument("Invalid AM generator parameters");
        }
        return std::make_unique<AmGeneratorFilter>(amplitude, carrier,
                                                   modulation, depth, duration);
    }

    if(type == "fm")
    {
        requireParameterCount(descriptor, 6);
        const double amplitude = parseDouble(descriptor, 1, "amplitude");
        const double carrier = parseDouble(descriptor, 2, "carrier_hz");
        const double modulation = parseDouble(descriptor, 3, "modulation_hz");
        const double deviation = parseDouble(descriptor, 4, "deviation_hz");
        const double duration = parseDouble(descriptor, 5, "duration_ms");
        if(amplitude < 0.0 || amplitude > 1.0 || carrier < 0.0 ||
           modulation <= 0.0 || deviation < 0.0 || duration < 0.0)
        {
            throw std::invalid_argument("Invalid FM generator parameters");
        }
        return std::make_unique<FmGeneratorFilter>(
            amplitude, carrier, modulation, deviation, duration);
    }

    throw std::invalid_argument("Unknown generator type: " + type);
}

CmdLineArgs2PipelineConverter::CmdLineArgs2PipelineConverter()
{
    addFilterProducer("ampl", createAmplFilter);
    addFilterProducer("normalize", createNormalizeFilter);
    addFilterProducer("silence", createSilenceFilter);
    addFilterProducer("timestretch", createTimeStretchFilter);
    addFilterProducer("lowpass", createLowPassFilter);
    addFilterProducer("generator", createGeneratorFilter);
}

Pipeline CmdLineArgs2PipelineConverter::createPipeline(
    const std::vector<FilterDescriptor>& descriptors) const
{
    Pipeline pipeline;
    for(const FilterDescriptor& descriptor: descriptors)
        pipeline.addFilter(createFilter(descriptor));
    return pipeline;
}

void CmdLineArgs2PipelineConverter::addFilterProducer(
    const std::string& filterName, FilterProducer producer)
{
    if(filterName.empty() || producer == nullptr)
    {
        throw std::invalid_argument(
            "Filter producer name and function must be valid");
    }
    _producers[filterName] = producer;
}

FilterProducer CmdLineArgs2PipelineConverter::getFilterProducer(
    const std::string& filterName) const
{
    const auto producer = _producers.find(filterName);
    return producer == _producers.end() ? nullptr : producer->second;
}

std::unique_ptr<IFilter> CmdLineArgs2PipelineConverter::createFilter(
    const FilterDescriptor& descriptor) const
{
    const FilterProducer producer = getFilterProducer(descriptor.name);
    if(producer == nullptr)
        throw std::invalid_argument("Unknown filter: " + descriptor.name);

    std::unique_ptr<IFilter> filter = producer(descriptor);
    if(!filter)
    {
        throw std::runtime_error("Filter producer failed for: " +
                                 descriptor.name);
    }
    return filter;
}
