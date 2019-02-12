//
//  wavfile.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef wavfile_h
#define wavfile_h

#include "riff.hpp"
#include "utils.hpp"

#include <string>
#include <vector>

#include <errno.h>
#include <stdio.h>

namespace mp3enc {

	template <class Platform>
	class WavFile {
		utils::InputFile _file;
		bool _msb;
		bool _valid;
		size_t _size;
		int _channels;
		int _sampleRate;

	public:
		WavFile(const std::string path)
		: _file(path.c_str())
		, _msb(false)
		, _size(0)
		, _channels(0)
		, _sampleRate(0) {
			_valid = parseRiffChunk() && parseFormatChunk() && parseDataChunk();
		}

		~WavFile() {
		}

		int ReadSamples(size_t num, short* dest) {
			if (!_valid)
				return -1;
			const size_t sampleSize = sizeof(short) * _channels;
			return _file.Read(dest, num * sampleSize) / sampleSize;
		}
		
	private:

		bool parseRiffChunk() {
			// RIFF chunk descriptor
			riff::ChunkDescriptor chunk;
		
			// Read chunk descriptor and validate it
			const bool validChunk = _file.ReadStruct(chunk) &&
				(0 == strncmp(chunk.id, "RIFF", 4) || 0 == strncmp(chunk.id, "RIFX", 4)) &&
				0 == strncmp(chunk.fmt, "WAVE", 4);
			if (!validChunk)
				return false;
		
			// Is the data stored in big endian (RIFX) format?
			_msb = (chunk.id[3] == 'X');
			return true;

		}
		
		bool parseFormatChunk() {
			// Format sub-chunk
			riff::FormatChunk chunk;
		
			// Read chunk descriptor and validate it
			const bool validChunk = _file.ReadStruct(chunk) &&
				0 == strncmp(chunk.id, "fmt ", 4) &&
				// Only raw 16 bit PCM format is supported
				utils::native_uint16<Platform>(chunk.size, _msb) == 16 &&
				utils::native_uint16<Platform>(chunk.audio_fmt, _msb) == 1 &&
				utils::native_uint16<Platform>(chunk.bits_per_sample, _msb) == 16;
			if (!validChunk)
				return false;
		
			// Save audio format parameters
			_channels = utils::native_uint16<Platform>(chunk.channels, _msb);
			_sampleRate = utils::native_uint32<Platform>(chunk.sample_rate, _msb);
			return true;
		}
		
		bool parseDataChunk() {
			// Data sub-chunk
			riff::DataChunk chunk;
		
			// Read chunk descriptor and validate it
			const bool validChunk = _file.ReadStruct(chunk) &&
				0 == strncmp(chunk.id, "data", 4);
			if (!validChunk)
				return false;
		
			// Save the following data size
			_size = utils::native_uint32<Platform>(chunk.size, _msb);
			return true;
		}
		
	}; // class WavFile
	
} // namespace mp3enc

#endif // #ifndef wavfile_h