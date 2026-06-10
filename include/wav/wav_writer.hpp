#pragma once

#include "wav/waveform.hpp"

#include <string>

class WavWriter
{
private:
    std::string _errorMessage;

public:
    enum class Result
    {
        Success,
        CannotCreateFile,
        UnsupportedFormat,
        WriteError
    };

    Result write(const std::string& fileName, const Waveform& waveform);

    const std::string& getErrorMessage() const;
};
