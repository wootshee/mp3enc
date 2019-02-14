//
//  wavfile.hpp - WAV input file
//
//  Copyright © 2019 Denis Shtyrov. All rights reserved.
//

#ifndef MP3ENC_WAVFILE_HPP
#define MP3ENC_WAVFILE_HPP

#include "file.hpp"
#include "utils.hpp"

namespace mp3enc {

    class WavFile {
        InputFile _file;
        // Does input file use big endian integer format?
        bool _bigendian;
        // Total number of samples stored in a file
        size_t _totalSamples;
        // Samples read so far
        size_t _samplesRead;
        // Input audio stream parameters
        int _channels;
        int _sampleRate;
        int _bitsPerSample;
    
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
            return _bitsPerSample;
        }

        // Read num samples from input WAV file and return
        // the actual number of samples read. Both 'num' argument
        // and method's return value specify number of samples
        // per channel (not the sum of samples in all channels)
        size_t ReadSamples(void* dest, size_t num);
        
    private:

        void parseRiffChunk();
        void parseFormatChunk();
        void parseDataChunk();
    }; // class WavFile
    
} // namespace mp3enc

#endif // #ifndef MP3ENC_WAVFILE_HPP
