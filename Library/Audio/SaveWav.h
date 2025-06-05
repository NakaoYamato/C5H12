#pragma once

#include <fstream>
#include <cstdint>
#include <xaudio2.h>

static void SaveWavFile(
    const std::string& filename,
    const BYTE* data,
    DWORD dataSize,
    const WAVEFORMATEX& fmt);