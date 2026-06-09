#include "application.hpp"


void printHelp(const char* applicationName)
{
    std::cout
        << "Usage:\n  " << applicationName
        << " [-i input.wav] [-o output.wav]"
        << " [-f filter [parameters...]]...\n\n"
        << "Examples:\n  " << applicationName
        << " -i input.wav -o output.wav -f ampl 0.8\n  "
        << applicationName
        << " -o output.wav -f generator sin 440 2000\n";
}

const char* filterStateMessage(FilterState state)
{
    switch (state) {
    case FilterState::Success:
        return "success";
    case FilterState::InvalidWaveform:
        return "invalid waveform";
    case FilterState::EmptyWaveform:
        return "empty waveform";
    case FilterState::InvalidParameter:
        return "invalid filter parameter";
    }
    return "unknown filter error";
}
