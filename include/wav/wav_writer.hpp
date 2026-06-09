#pragma once

#include "wav/waveform.hpp"

#include <string>


class WavWriter {
public:
    enum class Result {
        Success,
        CannotCreateFile,
        UnsupportedFormat,
        WriteError
    };

    Result write(const std::string& fileName, const Waveform& waveform);

    const std::string& getErrorMessage() const;

private:
    std::string _errorMessage;
};
