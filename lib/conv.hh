#ifndef PNGR_CONV_H_
#define PNGR_CONV_H_

#include <string_view>
#include <charconv>
#include <optional>

namespace conv
{
template <typename T>
[[nodiscard]] std::optional<T> string_to_integer(std::string_view const str) noexcept
{
	static_assert(std::is_integral<T>());

	if (T value; std::from_chars(str.begin(), str.end(), value).ec == std::errc{})
	{
		return value;
	}

	return std::nullopt;
}
}

#endif
