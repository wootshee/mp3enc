//
//  wavstream.hpp - WAV input stream
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_WAVSTREAM_HPP
#define MP3ENC_WAVSTREAM_HPP

#include "audiostream.hpp"
#include "riff.hpp"
#include "utils.hpp"

#include <string>
#include <vector>

#include <errno.h>
#include <stdio.h>

namespace mp3enc {

	template <class Platform>
	class WavInputStream : public AudioInputStream {
		utils::InputFile _file;
		bool _msb;
		bool _valid;
		// since original WAV format uses 32 bit size fields
		// they should also fit in ints
		int _totalSamples;
		int _channels;
		int _sampleRate;
		int _samplesRead;

		WavInputStream(const WavInputStream&);
		WavInputStream& operator=(const WavInputStream&);

	public:
		WavInputStream(const char* filepath)
		: _file(filepath)
		, _msb(false)
		, _totalSamples(0)
		, _channels(0)
		, _sampleRate(0)
		, _samplesRead(0) {
			_valid = parseRiffChunk() && parseFormatChunk() && parseDataChunk();
		}

		virtual ~WavInputStream() {
		}

		virtual int GetChannels() const {
			return _channels;
		}
		
		virtual int GetSampleRate() const {
			return _sampleRate;
		}
		
		virtual size_t GetTotalSamples() const {
			return static_cast<size_t>(_totalSamples);
		}

		virtual int GetBitsPerSample() const {
			return 16;
		}

		virtual int ReadSamples(void* dest, int num) {
			if (!_valid)
				return -1;
			const int sampleSize = sizeof(short) * _channels;
			const int read = _file.Read(dest, num * sampleSize) / sampleSize;
			_samplesRead += read;

			if (read == 0 && _samplesRead != _totalSamples) {
				// We reached EOF and read unexpected number of samples.
				// Input must be corrupt!
				return -1;
			}
			return read;
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
		
			// Save total number of samples in input stream
			_totalSamples = utils::native_uint32<Platform>(chunk.size, _msb) / (_channels * 2);
			return true;
		}
		
	}; // class WavFile
	
} // namespace mp3enc

#endif // #ifndef MP3ENC_WAVSTREAM_HPP