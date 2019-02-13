//
//  mp3stream.hpp - LAME based MP3 encoder
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_MP3STREAM_HPP
#define MP3ENC_MP3STREAM_HPP

#include "audiostream.hpp"
#include "utils.hpp"

#include <vector>

#include <lame.h>

namespace mp3enc {

	class Mp3OutputStream : public AudioOutputStream {
		class Lame {
			lame_global_flags* _lame;

			// Not interested in copying and assignment
			// for the sake of simplicity
			Lame(const Lame&);
			Lame& operator=(const Lame&);			
			
		public:
			Lame()
			: _lame(lame_init()) {
				assert(_lame);
				// Silence lame and let EncoderPool
				// report errors in controlled manner
				lame_set_errorf(_lame, NULL);
				lame_set_debugf(_lame, NULL);
				lame_set_msgf(_lame, NULL);
			}
			~Lame() {
				if (_lame) {
					lame_close(_lame);
					_lame = NULL;
				}
			}

			operator lame_global_flags*() {
				return _lame;
			}

		} _lame;

		utils::OutputFile _file;
		AudioInputStream* _input;
		size_t _samplesToRead;
		std::vector<unsigned char> _inputBuf;
		std::vector<unsigned char> _outputBuf;

		Mp3OutputStream(const Mp3OutputStream&);
		Mp3OutputStream& operator=(const Mp3OutputStream&);
		
	public:

		Mp3OutputStream(const char* filepath, size_t bufferSize = 128 * 1024)
		: _file(filepath)
		, _input(NULL)
		, _samplesToRead(0)
		, _outputBuf(bufferSize) {
		
		}

		virtual ~Mp3OutputStream() {
		}

		virtual void AttachToInputStream(AudioInputStream* input) {
			_input = input;

			// Prepare codec parameters
			lame_set_num_channels(_lame, input->GetChannels());
			lame_set_in_samplerate(_lame, input->GetSampleRate());
			lame_set_num_samples(_lame, input->GetTotalSamples());

			int res = lame_init_params(_lame);
			if (res < 0) {
				throw std::runtime_error("Failed to initialize LAME encoder");
			}

			_samplesToRead = lame_get_maximum_number_of_samples(_lame, _outputBuf.size());

			// Prepare input buffer
			_inputBuf.resize(input->GetChannels() * _samplesToRead * input->GetBitsPerSample() / 8);
		}
		
		virtual void Encode() {
			// Encode all input samples to output stream
			static const char* WRITE_ERROR = "Failed to write MP3 stream"; 
			int read = 0;
			while ((read = _input->ReadSamples(&_inputBuf[0], _samplesToRead)) > 0) {
				const int encoded =
					lame_encode_buffer_interleaved(
						_lame,
						reinterpret_cast<short*>(&_inputBuf[0]),
						read,
						&_outputBuf[0],
						_outputBuf.size());
				if (encoded < 0) {
					throw std::runtime_error("lame_encode_buffer_interleaved() failed");
				}
				if (encoded != _file.Write(&_outputBuf[0], encoded)) {
					throw std::runtime_error(WRITE_ERROR);
				}
			}

			// Flush last mp3 frame
			const int encoded = lame_encode_flush(_lame, &_outputBuf[0], _outputBuf.size());
			if (encoded < 0) {
				throw std::runtime_error("lame_encode_flush() failed");
			}

			if (encoded != _file.Write(&_outputBuf[0], encoded)) {
				throw std::runtime_error(WRITE_ERROR);
			}
		}
	}; // class OutputMp3Stream
} // namespace mp3enc

#endif // #ifndef MP3ENC_MP3STREAM_HPP