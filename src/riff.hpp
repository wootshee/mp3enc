#ifndef riff_hpp
#define riff_hpp

#include <stdint.h>

namespace mp3enc {
namespace riff {

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

} // namespace riff {
} // namespace mp3enc {

#endif // #ifndef riff_pp