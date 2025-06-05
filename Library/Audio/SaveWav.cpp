#include "SaveWav.h"

void SaveWavFile(
    const std::string& filename,
    const BYTE* data, 
    DWORD dataSize, 
    const WAVEFORMATEX& fmt)
{
    std::ofstream out(filename, std::ios::binary);

    // RIFFヘッダー
    out.write("RIFF", 4);
    uint32_t chunkSize = 36 + dataSize;
    out.write(reinterpret_cast<const char*>(&chunkSize), 4);
    out.write("WAVE", 4);

    // fmtチャンク
    out.write("fmt ", 4);
    uint32_t subchunk1Size = 16; // PCM
    out.write(reinterpret_cast<const char*>(&subchunk1Size), 4);
    out.write(reinterpret_cast<const char*>(&fmt.wFormatTag), 2);
    out.write(reinterpret_cast<const char*>(&fmt.nChannels), 2);
    out.write(reinterpret_cast<const char*>(&fmt.nSamplesPerSec), 4);
    out.write(reinterpret_cast<const char*>(&fmt.nAvgBytesPerSec), 4);
    out.write(reinterpret_cast<const char*>(&fmt.nBlockAlign), 2);
    out.write(reinterpret_cast<const char*>(&fmt.wBitsPerSample), 2);

    // dataチャンク
    out.write("data", 4);
    out.write(reinterpret_cast<const char*>(&dataSize), 4);
    out.write(reinterpret_cast<const char*>(data), dataSize);

    out.close();
}
