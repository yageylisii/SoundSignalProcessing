#pragma once

#include "parser/console_parser.hpp"
#include "pipeline/cmd_line_args_2_pipeline_converter.hpp"
#include "wav/wav_reader.hpp"
#include "wav/wav_writer.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>

constexpr int SuccessExitCode = EXIT_SUCCESS;
constexpr int FailureExitCode = EXIT_FAILURE;
constexpr int UnknownFailureExitCode = 2;

void printHelp(const char* applicationName);
const char* filterStateMessage(FilterState state);
