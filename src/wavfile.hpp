//
//  wavstream.hpp - WAV input stream
//  mp3enc
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_WAVSTREAM_HPP
#define MP3ENC_WAVSTREAM_HPP

#include "riff.hpp"
#include "utils.hpp"

#include <stdexcept>
#include <string>

namespace mp3enc {

	class WavFile {
		utils::InputFile _file;
		bool _bigendian;
		// since original WAV format uses 32 bit size fields
		// they should also fit in ints
		size_t _totalSamples;
		size_t _samplesRead;
		int _channels;
		int _sampleRate;
	
		WavFile(const WavFile&);
		WavFile& operator=(const WavFile&);

	public:
		WavFile(const char* filepath);
		~WavFile() {
		}

		int GetChannels() const {
			return _channels;
		}
		
		int GetSampleRate() const {
			return _sampleRate;
		}
		
		size_t GetTotalSamples() const {
			return static_cast<size_t>(_totalSamples);
		}

		int GetBitsPerSample() const {
			return 16;
		}

		size_t ReadSamples(void* dest, size_t num);
		
	private:

		void parseRiffChunk();
		void parseFormatChunk();
		void parseDataChunk();
	}; // class WavFile
	
} // namespace mp3enc

#endif // #ifndef MP3ENC_WAVSTREAM_HPP