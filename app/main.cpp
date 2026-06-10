#include "application.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {  // create parser object
        ArgsParser parser;
        // fill the parser object with command line arguments
        const ArgsParser::Result parseResult = parser.parse(argc, argv);
        if(parseResult == ArgsParser::Result::NoArguments)  // if no arguments
        {
            printHelp(argv[0]);
            return SuccessExitCode;
        }
        if(parseResult !=
           ArgsParser::Result::Success)  // check correct arguments
        {
            std::cerr << "Error: " << parser.getErrorMessage() << '\n';
            return FailureExitCode;
        }

        // create waveform object and set required parameters
        Waveform waveform;
        waveform.setSampleRate(44100);
        waveform.setChannelCount(1);
        waveform.setBitsPerSample(16);

        // if input file is not empty
        if(!parser.getInFileName().empty())
        {
            // if input file exists then create WavReader and fill it with the
            // file data
            WavReader wavReader;
            if(wavReader.read(parser.getInFileName(), waveform) !=
               WavReader::Result::Success)
            {
                std::cerr << "Error: " << wavReader.getErrorMessage() << '\n';
                return FailureExitCode;
            }
        }

        // transfer filter descriptors from command line arguments to smart
        // pointers on a funcs
        CmdLineArgs2PipelineConverter converter;
        // create pipeline object and fill it
        Pipeline pipeline =
            converter.createPipeline(parser.getFilterDescriptors());

        // apply pipline filters to the waveform
        const FilterState filterState = pipeline.apply(&waveform);
        if(filterState != FilterState::Success)
        {
            std::cerr << "Error: filter " << pipeline.getFailedFilterIndex()
                      << " failed: " << filterStateMessage(filterState) << '\n';
            return FailureExitCode;
        }

        // if output file is not empty
        if(!parser.getOutFileName().empty())
        {
            // create WavWriter and write the output file
            WavWriter wavWriter;
            // write waveform to the output file
            if(wavWriter.write(parser.getOutFileName(), waveform) !=
               WavWriter::Result::Success)
            {
                std::cerr << "Error: " << wavWriter.getErrorMessage() << '\n';
                return FailureExitCode;
            }
            std::cout << "Output: " << parser.getOutFileName() << '\n';
        }

        std::cout << "Applied filters: " << pipeline.getFilterCount() << '\n';
        return SuccessExitCode;
    }
    catch(const std::exception& error)
    {
        std::cerr << "Error: " << error.what() << '\n';
        return FailureExitCode;
    }
    catch(...)
    {
        std::cerr << "Error: unknown exception\n";
        return UnknownFailureExitCode;
    }
}
