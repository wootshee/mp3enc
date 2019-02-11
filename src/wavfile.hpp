//
//  wavfile.hpp
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef wavfile_h
#define wavfile_h

#include <string>
#include <vector>

#include <errno.h>
#include <stdio.h>

namespace mp3enc {

	template <class Platform>
	class WavFile {
		FILE* _file;
		int _error;
		bool _msb;
		size_t _size;
		int _channels;
		int _sampleRate;

	public:
		WavFile(const std::string path)
		: _file(fopen(path.c_str(), "rb"))
		, _msb(false)
		, _size(0)
		, _channels(0)
		, _sampleRate(0) {
			if (!_file) {
				_error = errno;
			}
			const bool ok = parseRiffChunk() && parseFormatChunk() && parseDataChunk();
			if (!ok) {
				_error = EIO;
			}
		}

		~WavFile() {
			if (_file) {
				fclose(_file);
			}
		}

		size_t ReadSamples(size_t num, short* dest) {
			const size_t sampleSize = sizeof(short) * _channels;
			const size_t read = fread(dest, sampleSize, num, _file);
			if (read != num) {
				if (ferror(_file)) {
					_error = EIO;
				}
			}
			return read;
		}

		int Error() const {
			return _error;
		}
		
	private:
	
		bool parseRiffChunk() {
			// RIFF chunk descriptor
			#pragma pack(push, 1)
			struct {
				char id[4];
				uint32_t size;
				char fmt[4];
			} chunk;
			#pragma pack(pop)
		
			// Read chunk descriptor and validate it
			size_t read = fread(&chunk, 1, sizeof(chunk), _file);
			const bool validChunk =
				read == sizeof(chunk) &&
				(0 == strncmp(chunk.id, "RIFF", 4) || 0 == strncmp(chunk.id, "RIFX", 4)) &&
				0 == strncmp(chunk.fmt, "WAVE", 4);
			if (!validChunk)
				return false;
		
			// Is the data stored in big endian format?
			_msb = (chunk.id[3] == 'X');
			return true;

		}
		
		bool parseFormatChunk() {
			// Format sub-chunk
			#pragma pack(push, 1)
			struct {
				char id[4];
				uint32_t size;
				uint16_t audio_fmt;
				uint16_t channels;
				uint32_t sample_rate;
				uint32_t byte_rate;
				uint16_t block_align;
				uint16_t bits_per_sample;
			} chunk;
			#pragma pack(pop)
		
			// Read chunk descriptor and validate it
			size_t read = fread(&chunk, 1, sizeof(chunk), _file);
			const bool validChunk = 
				read == sizeof(chunk) &&
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
			#pragma pack(push, 1)
			struct {
				char id[4];
				uint32_t size;
			} chunk;
			#pragma pack(pop)
		
			// Read chunk descriptor and validate it
			size_t read = fread(&chunk, 1, sizeof(chunk), _file);
			const bool validChunk = 
				read == sizeof(chunk) &&
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