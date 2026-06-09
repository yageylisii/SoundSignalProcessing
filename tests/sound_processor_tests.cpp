#include "filters/filters.hpp"
#include "parser/console_parser.hpp"
#include "pipeline/cmd_line_args_2_pipeline_converter.hpp"
#include "wav/wav_reader.hpp"
#include "wav/wav_writer.hpp"

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

Waveform makeWaveform(std::vector<std::int16_t> samples)
{
    Waveform waveform;
    waveform.setSampleRate(44100);
    waveform.setChannelCount(1);
    waveform.setBitsPerSample(16);
    waveform.setSamples(std::move(samples));
    return waveform;
}

TEST_CASE("Command line parser", "[parser]")
{
    ArgsParser parser;

    SECTION("No arguments")
    {
        char application[] = "sound_processor";
        char* arguments[] = {application};

        REQUIRE(parser.parse(1, arguments) ==
                ArgsParser::Result::NoArguments);
    }

    SECTION("Input output and filter arguments")
    {
        char application[] = "sound_processor";
        char inputOption[] = "-i";
        char inputName[] = "input.wav";
        char outputOption[] = "-o";
        char outputName[] = "output.wav";
        char filterOption[] = "-f";
        char filterName[] = "ampl";
        char factor[] = "0.8";
        char* arguments[] = {application,  inputOption, inputName,
                             outputOption, outputName, filterOption,
                             filterName,   factor};

        REQUIRE(parser.parse(8, arguments) == ArgsParser::Result::Success);
        REQUIRE(parser.getInFileName() == "input.wav");
        REQUIRE(parser.getOutFileName() == "output.wav");
        REQUIRE(parser.getFilterDescriptors().size() == 1);
        REQUIRE(parser.getFilterDescriptors()[0].name == "ampl");
        REQUIRE(parser.getFilterDescriptors()[0].parameters ==
                std::vector<std::string>{"0.8"});
    }
}

TEST_CASE("Transform filters", "[filters][transform]")
{
    SECTION("Amplification clamps samples")
    {
        Waveform waveform = makeWaveform({20000, -20000});
        AmplFilter filter(2.0);

        REQUIRE(filter.apply(&waveform) == FilterState::Success);
        REQUIRE(waveform.getSamples() ==
                std::vector<std::int16_t>{32767, -32768});
    }

    SECTION("Normalization changes peak amplitude")
    {
        Waveform waveform = makeWaveform({-100, 50});
        NormalizeFilter filter;

        REQUIRE(filter.apply(&waveform) == FilterState::Success);
        REQUIRE(waveform.getSamples() ==
                std::vector<std::int16_t>{-32767, 16384});
    }

    SECTION("Silence inserts a silent interval")
    {
        Waveform waveform = makeWaveform({1, 2, 3});
        waveform.setSampleRate(10);
        SilenceFilter filter(TimeUnit::Seconds, 0.1, 0.2);

        REQUIRE(filter.apply(&waveform) == FilterState::Success);
        REQUIRE(waveform.getSamples() ==
                std::vector<std::int16_t>{1, 0, 0, 2, 3});
    }

    SECTION("Time stretch increases sample count")
    {
        Waveform waveform = makeWaveform({0, 10});
        TimeStretchFilter filter(2.0);

        REQUIRE(filter.apply(&waveform) == FilterState::Success);
        REQUIRE(waveform.getSamples() ==
                std::vector<std::int16_t>{0, 5, 10, 10});
    }

    SECTION("Low pass smooths samples")
    {
        Waveform waveform = makeWaveform({0, 9, 0});
        LowPassFilter filter(3);

        REQUIRE(filter.apply(&waveform) == FilterState::Success);
        REQUIRE(waveform.getSamples() ==
                std::vector<std::int16_t>{3, 3, 3});
    }
}

TEST_CASE("Sound generators", "[filters][generator]")
{
    Waveform waveform;

    SECTION("Sine generator")
    {
        SinGeneratorFilter filter(440.0, 10.0);
        REQUIRE(filter.apply(&waveform) == FilterState::Success);
    }

    SECTION("AM generator")
    {
        AmGeneratorFilter filter(0.5, 440.0, 5.0, 0.5, 10.0);
        REQUIRE(filter.apply(&waveform) == FilterState::Success);
    }

    SECTION("FM generator")
    {
        FmGeneratorFilter filter(0.5, 440.0, 5.0, 20.0, 10.0);
        REQUIRE(filter.apply(&waveform) == FilterState::Success);
    }

    REQUIRE(waveform.getSampleRate() == 44100);
    REQUIRE(waveform.getChannelCount() == 1);
    REQUIRE(waveform.getBitsPerSample() == 16);
    REQUIRE(waveform.getSampleCount() == 441);
}

TEST_CASE("Pipeline applies filters in order", "[pipeline]")
{
    const std::vector<FilterDescriptor> descriptors = {
        {"ampl", {"2"}}, {"normalize", {"0.5"}}};

    CmdLineArgs2PipelineConverter converter;
    Pipeline pipeline = converter.createPipeline(descriptors);
    Waveform waveform = makeWaveform({1000, -2000});

    REQUIRE(pipeline.getFilterCount() == 2);
    REQUIRE(pipeline.apply(&waveform) == FilterState::Success);
    REQUIRE(waveform.getSamples()[1] == -16384);
}

TEST_CASE("Converter rejects an unknown filter", "[pipeline][errors]")
{
    CmdLineArgs2PipelineConverter converter;

    REQUIRE_THROWS_AS(converter.createPipeline({{"unknown", {}}}),
                      std::invalid_argument);
}

TEST_CASE("WAV write and read preserves sound", "[wav]")
{
    const std::string fileName = "/tmp/sound_processor_writer_test.wav";
    Waveform source = makeWaveform({-32768, -1000, 0, 1000, 32767});
    WavWriter writer;
    WavReader reader;

    REQUIRE(writer.write(fileName, source) == WavWriter::Result::Success);

    Waveform result;
    REQUIRE(reader.read(fileName, result) == WavReader::Result::Success);
    REQUIRE(result.getSampleRate() == source.getSampleRate());
    REQUIRE(result.getChannelCount() == source.getChannelCount());
    REQUIRE(result.getBitsPerSample() == source.getBitsPerSample());
    REQUIRE(result.getSamples() == source.getSamples());
}

TEST_CASE("Empty WAV can be written and read", "[wav]")
{
    const std::string fileName = "/tmp/sound_processor_empty_test.wav";
    Waveform source = makeWaveform({});
    WavWriter writer;
    WavReader reader;

    REQUIRE(writer.write(fileName, source) == WavWriter::Result::Success);

    Waveform result;
    REQUIRE(reader.read(fileName, result) == WavReader::Result::Success);
    REQUIRE(result.getSamples().empty());
}
