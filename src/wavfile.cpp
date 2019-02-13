//
//  wavfile.cpp - WAV input file
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_WAVSTREAM_HPP
#define MP3ENC_WAVSTREAM_HPP

#include "wavfile.hpp"

#include "utils.hpp"

#include <stdexcept>
#include <string>

namespace {
//
// Chunk structures defined by RIFF(X) format
//

#pragma pack(push, 1)

struct ChunkDescriptor {
	char id[4];
	uint32_t size;
	char fmt[4];
};

struct FormatChunk {
	char id[4];
	uint32_t size;
	uint16_t audio_fmt;
	uint16_t channels;
	uint32_t sample_rate;
	uint32_t byte_rate;
	uint16_t block_align;
	uint16_t bits_per_sample;
};

struct DataChunk {
	char id[4];
	uint32_t size;
};

#pragma pack(pop)
} // namespace

namespace mp3enc {

WavFile::WavFile(const char* filepath)
: _file(filepath)
, _bigendian(false)
, _totalSamples(0)
, _samplesRead(0)
, _channels(0)
, _sampleRate(0) {
    parseRiffChunk();
    parseFormatChunk();
    parseDataChunk();
}

size_t WavFile::ReadSamples(void* dest, size_t num) {
    if (_samplesRead == _totalSamples) {
        // Audio stream has been fully consumed
        return 0;
    }
    
    if (num > _totalSamples - _samplesRead) {
        num = _totalSamples - _samplesRead;
    }

    const int sampleSize = sizeof(short) * _channels;
    const int read = _file.Read(dest, num * sampleSize) / sampleSize;
    _samplesRead += read;

    if (read == 0 && _samplesRead != _totalSamples) {
        // We reached EOF and read unexpected number of samples.
        // Input must be corrupt!
        throw std::runtime_error("Unexpected end of WAV stream");
    }
    return read;
}
		
void WavFile::parseRiffChunk() {
    // RIFF chunk descriptor
    ChunkDescriptor chunk;

    // Read chunk descriptor and validate it
    const bool validChunk = _file.ReadStruct(chunk) &&
        (0 == strncmp(chunk.id, "RIFF", 4) || 0 == strncmp(chunk.id, "RIFX", 4)) &&
        0 == strncmp(chunk.fmt, "WAVE", 4);
    if (!validChunk)
        throw std::runtime_error("Unsupported RIFF type");

    // Is the data stored in big endian (RIFX) format?
    _bigendian = (chunk.id[3] == 'X');

}

void WavFile::parseFormatChunk() {
    // Format sub-chunk
    FormatChunk chunk;

    // Read chunk descriptor and validate it
    const bool validChunk = _file.ReadStruct(chunk) &&
        0 == strncmp(chunk.id, "fmt ", 4) &&
        // Only raw 16 bit PCM format is supported
        utils::native_uint16(chunk.size, _bigendian) == 16 &&
        utils::native_uint16(chunk.audio_fmt, _bigendian) == 1 &&
        utils::native_uint16(chunk.bits_per_sample, _bigendian) == 16;
    if (!validChunk)
        throw std::runtime_error("Unsupported WAV format");

    // Save audio format parameters
    _channels = utils::native_uint16(chunk.channels, _bigendian);
    _sampleRate = utils::native_uint32(chunk.sample_rate, _bigendian);
}

void WavFile::parseDataChunk() {
    // Data sub-chunk
    DataChunk chunk;

    // Read chunk descriptor and validate it
    const bool validChunk = _file.ReadStruct(chunk) &&
        0 == strncmp(chunk.id, "data", 4);
    if (!validChunk)
        throw std::runtime_error("Invalid RIFF data chunk");

    // Save total number of samples in input stream
    _totalSamples = utils::native_uint32(chunk.size, _bigendian) / (_channels * 2);
}
	
} // namespace mp3enc

#endif // #ifndef MP3ENC_WAVSTREAM_HPP