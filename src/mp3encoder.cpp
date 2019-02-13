//
//  mp3stream.cpp - LAME based MP3 encoder
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
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
    void encode(
        WavFile& input,
        std::vector<unsigned char>& inBuf,
        std::vector<unsigned char>& outBuf,
        const char* outpath) {

        OutputFile output(outpath);

        const size_t samplesToRead = 16384;
        outBuf.resize(size_t(1.25 * samplesToRead + 7200));

        // Prepare codec parameters (use default quality settings)
        Lame encoder;
        lame_set_num_channels(encoder, input.GetChannels());
        lame_set_in_samplerate(encoder, input.GetSampleRate());
        lame_set_out_samplerate(encoder, input.GetSampleRate());
        lame_set_num_samples(encoder, input.GetTotalSamples());

        int res = lame_init_params(encoder);
        if (res < 0) {
            throw std::runtime_error("Failed to initialize LAME encoder");
        }

        // Allocate input buffer of sufficient size
        size_t requiredSize = input.GetChannels() * samplesToRead * input.GetBitsPerSample() / 8;
        if (inBuf.size() < requiredSize) {
            inBuf.resize(requiredSize);
        }

        // Encode all input samples to output stream
        const bool mono = input.GetChannels() == 1;
        size_t read = 0;
        while ((read = input.ReadSamples(&inBuf[0], samplesToRead)) > 0) {
            int encoded = 0;
            if (mono) {
                encoded = lame_encode_buffer(
                    encoder,
                    reinterpret_cast<short*>(&inBuf[0]),
                    reinterpret_cast<short*>(&inBuf[0]),
                    read,
                    &outBuf[0],
                    static_cast<int>(outBuf.size()));
            } else {
                encoded = lame_encode_buffer_interleaved(
                    encoder,
                    reinterpret_cast<short*>(&inBuf[0]),
                    read,
                    &outBuf[0],
                    static_cast<int>(outBuf.size()));
            }
            if (encoded < 0) {
                throw std::runtime_error("lame_encode_buffer() failed");
            }
            if (encoded != output.Write(&outBuf[0], encoded)) {
                throw std::runtime_error(WRITE_ERROR);
            }
        }

        // Flush last mp3 frame
        const int encoded = lame_encode_flush(encoder, &outBuf[0], outBuf.size());
        if (encoded < 0) {
            throw std::runtime_error("lame_encode_flush() failed");
        }

        if (encoded != output.Write(&outBuf[0], encoded)) {
            throw std::runtime_error(WRITE_ERROR);
        }
    }
} // namespace mp3enc
