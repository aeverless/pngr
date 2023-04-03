#ifndef PNGR_ARCH_H_
#define PNGR_ARCH_H_

#include <cstdint>


namespace arch
{
enum class Endian
{
	Big,
	Little,
};

[[nodiscard]] static inline Endian byte_order() noexcept
{
	uint16_t const x = 1;
	return reinterpret_cast<uint8_t const*>(&x)[0] ? Endian::Little : Endian::Big;
}

[[nodiscard]] static inline bool is_big_endian() noexcept
{
	return byte_order() == Endian::Big;
}

[[nodiscard]] static inline bool is_little_endian() noexcept
{
	return !is_big_endian();
}
}

#endif
