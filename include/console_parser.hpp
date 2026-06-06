#pragma once

#include <string>
#include <vector>


struct FilterDescriptor {
    std::string name;
    std::vector<std::string> parameters;
};

class ArgsParser {
private:
    std::string _inFileName;
    std::string _outFileName;
    std::vector<FilterDescriptor> _filterDescriptors;
    std::string _errorMessage;
public:
    enum class Result {
        Success,
        InvalidArguments
    };

    ArgsParser() = default;

    Result parse(int argc, char* argv[]);

    const std::string& getInFileName() const;
    const std::string& getOutFileName() const;
    const std::vector<FilterDescriptor>& getFilterDescriptors() const;
    const std::string& getErrorMessage() const;
};
