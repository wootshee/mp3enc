//
//  mp3stream.hpp - LAME based MP3 encoder
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#ifndef MP3ENC_MP3STREAM_HPP
#define MP3ENC_MP3STREAM_HPP

#include "encoder.hpp"
#include "utils.hpp"

#include <lame.h>

namespace mp3enc {

	class OutputMp3Stream : public OutputAudioStream {
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
		AudioInput* _input;
		size_t _samplesToRead;
		std::vector<unsigned char> _inputBuf;
		std::vector<unsigned char> _outputBuf;
		
	public:

		OutputMp3Stream(const char* filepath, size_t bufferSize = 128 * 1024 * 1024)
		: _file(filepath)
		, _input(NULL)
		, _samplesToRead(0)
		, _outputBuf(bufferSize) {
		
		}

		virtual ~OutputMp3Stream() {
		}

		virtual int AttachToInput(AudioInput* input) {
			// Prepare codec parameters
			lame_set_num_channels(_lame, input->GetNumChannels());
			lame_set_in_sample_rate(_lame, input->GetSampleRate());
			lame_set_num_samples(_lame, input->GetTotalSamples());

			_samplesToRead = lame_get_maximum_number_of_samples(_lame, _outputBuf.size());

			// Prepare input buffer (1 second of audio stream seems to be
			// a good starting point)
			_inputBuf.resize(input->GetNumChannels() * _samplesInBuffer * input->GetBitsPerSample() / 8);

		}
		
		virtual int Encode() {
			// Encode all input samples to output stream
			int res = 0;
			while ((res = _input->ReadSamples(&_inputBuf[0], _samplesToRead) > 0) {
				const int encoded =
					lame_encode_buffer_interleaved(
						_lame,
						reinterpret_cast<short*>(&_inputBuf[0]),
						_samplesToRead,
						&_outputBuf[0],
						_outputBuf.size());
				if (encoded != _file.Write(&_outputBuf[0], encoded)) {
					return EIO;
				}
			}

			if (res == -1) {
				return EIO;
			}
			return 0;
		}
		
	
	}; // class OutputMp3Stream
	
	
} // namespace mp3enc

#endif // #ifndef MP3ENC_MP3STREAM_HPP