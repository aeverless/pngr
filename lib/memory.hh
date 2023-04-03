#ifndef PNGR_MEMORY_H_
#define PNGR_MEMORY_H_

#include "./arch.hh"


namespace memory
{
template <typename T>
[[nodiscard]] T const& swap_byte_order(
	T const& value,
	size_t const swap_count = 0,
	size_t const zero_init_count = 0
) noexcept
{
	size_t const size = zero_init_count ? zero_init_count : sizeof(value);
	char* const result(new char[size]{});

	size_t const length = swap_count ? swap_count : size;
	for (size_t i = 0; i < length; i++)
	{
		result[i] = reinterpret_cast<char const* const>(&value)[length - i - 1];
	}

	return *reinterpret_cast<T*>(result);
}

/// Assuming that given value is in native-endian order, convert it to big-endian order.
template <typename T>
[[nodiscard]] static inline T const& to_big_endian(
	T const& value,
	size_t const swap_count = 0,
	size_t const zero_init_count = 0
) noexcept
{
	return arch::is_big_endian() ? value : swap_byte_order(value, swap_count, zero_init_count);
}

/// Assuming that given value is in native-endian order, convert it to little-endian order.
template <typename T>
[[nodiscard]] static inline T const& to_little_endian(
	T const& value,
	size_t const swap_count = 0,
	size_t const zero_init_count = 0
) noexcept
{
	return arch::is_little_endian() ? value : swap_byte_order(value, swap_count, zero_init_count);
}
}

#endif
