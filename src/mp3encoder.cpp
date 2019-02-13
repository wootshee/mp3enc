//
//  mp3stream.cpp - LAME based MP3 encoder
//
//  Created by Denis Shtyrov on 10.02.19.
//  Copyright © 2019 wootshee. All rights reserved.
//

#include "mp3encoder.hpp"
#include "file.hpp"

#include <stdexcept>

#include <lame.h>

namespace {

    static const char* WRITE_ERROR = "Failed to write MP3 stream"; 

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
    }; // class Lame;
} // namespace

namespace mp3enc {
    // Encode WAV PCM data to MP3 stream
    void encode(WavFile& input, const char* outpath, size_t bufferSize) {
        OutputFile output(outpath);

        // Allocate output buffer
        std::vector<unsigned char> outputBuf(bufferSize);

        // Prepare codec parameters
        Lame encoder;
        lame_set_num_channels(encoder, input.GetChannels());
        lame_set_in_samplerate(encoder, input.GetSampleRate());
        lame_set_num_samples(encoder, input.GetTotalSamples());

        int res = lame_init_params(encoder);
        if (res < 0) {
            throw std::runtime_error("Failed to initialize LAME encoder");
        }

        // Allocate input buffer of sufficient size
        size_t samplesToRead = lame_get_maximum_number_of_samples(encoder, outputBuf.size());
        std::vector<unsigned char> inputBuf(
            input.GetChannels() * samplesToRead * input.GetBitsPerSample() / 8);

        // Encode all input samples to output stream
        size_t read = 0;
        while ((read = input.ReadSamples(&inputBuf[0], samplesToRead)) > 0) {
            const int encoded =
                lame_encode_buffer_interleaved(
                    encoder,
                    reinterpret_cast<short*>(&inputBuf[0]),
                    read,
                    &outputBuf[0],
                    outputBuf.size());
            if (encoded < 0) {
                throw std::runtime_error("lame_encode_buffer_interleaved() failed");
            }
            if (encoded != output.Write(&outputBuf[0], encoded)) {
                throw std::runtime_error(WRITE_ERROR);
            }
        }

        // Flush last mp3 frame
        const int encoded = lame_encode_flush(encoder, &outputBuf[0], outputBuf.size());
        if (encoded < 0) {
            throw std::runtime_error("lame_encode_flush() failed");
        }

        if (encoded != output.Write(&outputBuf[0], encoded)) {
            throw std::runtime_error(WRITE_ERROR);
        }
    }
} // namespace mp3enc
