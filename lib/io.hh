#ifndef PNGR_IO_H_
#define PNGR_IO_H_

#include "memory.hh"

#include <istream>
#include <ostream>


namespace io
{
template <typename T>
static inline std::istream& read(std::istream& is, T& destination, std::size_t const count = 0)
{
	return is.read(reinterpret_cast<char*>(&destination), count ? count : sizeof(destination));
}

template <typename T>
static inline std::istream& read_endian(std::istream& is, T& destination, arch::Endian const endian, std::size_t const count = 0)
{
	std::istream& ret = read(is, destination, count);
	destination = endian == arch::Endian::Big ? memory::to_big_endian(destination, 0, count)
	                                          : memory::to_little_endian(destination, 0, count);
	return ret;
}

template <typename T>
static inline std::ostream& write(std::ostream& os, T const& value, std::size_t const count = 0)
{
	return os.write(reinterpret_cast<char const* const>(&value), count ? count : sizeof(value));
}
}

#endif
