#include "console_parser.hpp"
#include "wav_reader.hpp"
#include "wav_writer.hpp"

#include <iostream>


int main(int argc, char* argv[])
{
    ArgsParser parser = {};
    if (parser.parse(argc, argv) != ArgsParser::Result::Success) {
        std::cerr << "Error: " << parser.getErrorMessage() << '\n';
        return EXIT_FAILURE;
    }

    const std::string& input = parser.getInFileName();
    const std::string& output = parser.getOutFileName();

    if (input.empty()) {
        std::cerr << "Error: input WAV file is not specified\n";
        return EXIT_FAILURE;
    }

    WavReader wavReader;
    Waveform waveform;
    if (wavReader.read(input, waveform) != WavReader::Result::Success) {
        std::cerr << "Error: " << wavReader.getErrorMessage() << '\n';
        return EXIT_FAILURE;
    }

    if (output.empty()) {
        std::cerr << "Error: output WAV file is not specified\n";
        return EXIT_FAILURE;
    }
    WavWriter wavWriter;
    if (wavWriter.write(output, waveform) != WavWriter::Result::Success) {
        std::cerr << "Error: " << wavWriter.getErrorMessage() << '\n';
        return EXIT_FAILURE;
    }

    std::cout << "Input: " << input << '\n';
    std::cout << "Output: " << output << '\n';
    std::cout << "Sample rate: " << waveform.sampleRate << " Hz\n";
    std::cout << "Channels: " << waveform.channelCount << '\n';
    std::cout << "Bits per sample: " << waveform.bitsPerSample << '\n';
    std::cout << "Samples: " << waveform.getSampleCount() << '\n';
    std::cout << "Duration: " << waveform.getDurationSeconds() << " sec\n";

    return EXIT_SUCCESS;
}
