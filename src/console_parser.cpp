#include "console_parser.hpp"

#include <utility>


namespace {

bool isOption(const std::string& argument)
{
    return argument == "-i" || argument == "-o" || argument == "-f";
}

} // namespace

// parse command line arg and store the result in ArgsParser
ArgsParser::Result ArgsParser::parse(int argc, char* argv[])
{
    _inFileName.clear();
    _outFileName.clear();
    _filterDescriptors.clear();
    _errorMessage.clear();

    int idx = 1;
    while (idx < argc) {
        const std::string option = argv[idx];

        if (option == "-i" || option == "-o") {
            // check that the next argument exists and is not another option
            if (idx + 1 >= argc || isOption(argv[idx + 1])) {
                _errorMessage = option + " requires a file path";
                return Result::InvalidArguments;
            }

            // write the file path to the parser
            if (option == "-i") {
                _inFileName = argv[idx + 1];
            } else {
                _outFileName = argv[idx + 1];
            }
            // jump on the next option
            idx += 2;
        } else if (option == "-f") {
            // check that the next argument exists and is not another option
            if (idx + 1 >= argc || isOption(argv[idx + 1])) {
                _errorMessage = "-f requires a filter name";
                return Result::InvalidArguments;
            }

            FilterDescriptor descriptor;
            descriptor.name = argv[idx + 1];
            idx += 2;

            // parse filters parametrs until the next option or the end of arguments
            while (idx < argc && !isOption(argv[idx])) {
                descriptor.parameters.emplace_back(argv[idx]);
                ++idx;
            }

            _filterDescriptors.push_back(std::move(descriptor));
        } else {
            _errorMessage = "Unknown argument: " + option;
            return Result::InvalidArguments;
        }
    }

    return Result::Success;
}

const std::string& ArgsParser::getErrorMessage() const
{
    return _errorMessage;
}

const std::string& ArgsParser::getInFileName() const
{
    return _inFileName;
}

const std::string& ArgsParser::getOutFileName() const
{
    return _outFileName;
}

const std::vector<FilterDescriptor>& ArgsParser::getFilterDescriptors() const
{
    return _filterDescriptors;
}
