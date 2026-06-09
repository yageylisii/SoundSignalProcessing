#pragma once

#include "wav/waveform.hpp"

#include <cstddef>
#include <cstdint>


constexpr std::uint32_t GeneratorSampleRate = 44100;
constexpr std::uint16_t GeneratorChannelCount = 1;
constexpr std::uint16_t GeneratorBitsPerSample = 16;
constexpr double Pi = 3.14159265358979323846;
constexpr double MaximumAmplitude = 32767.0;

bool isValidWaveform(const Waveform* sound);
std::int16_t clampSample(double value);
bool calculateGeneratorSize(double durationMs, std::size_t& sampleCount);
void prepareGeneratedWaveform(Waveform& sound, std::size_t sampleCount);
