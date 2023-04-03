#ifndef PNGR_COLOR_H_
#define PNGR_COLOR_H_

#include <cstdint>


namespace color
{
using Value = uint64_t;
using ChannelIndex = size_t;

namespace grayscale
{
enum Channel : ChannelIndex
{
	G,
	A,
};
}

namespace truecolor
{
enum Channel : ChannelIndex
{
	R,
	G,
	B,
	A,
};
}
}

#endif
