#include "./cli.hh"
#include "../lib/conv.hh"

#include <stdexcept>


namespace cli
{
[[nodiscard]] bool is_hex(std::string_view const str) noexcept
{
	std::size_t const length = str.length();

	if (length <= 2 || str.substr(0, 2) != "0x")
	{
		return false;
	}

	for (auto it = std::next(str.begin(), 2); it != str.end(); ++it)
	{
		if (char const& ch = std::tolower(*it); !std::isdigit(ch) && ('a' > ch || ch > 'f'))
		{
			return false;
		}
	}

	return true;
}

[[nodiscard]] math::Vector string_to_vector(std::string_view const str, std::string_view const delimiter)
{
	std::size_t const delimiter_index = str.find(delimiter);

	if (delimiter_index == std::string_view::npos)
	{
		throw std::invalid_argument("no delimiter found in str");
	}

	return math::Vector{
		conv::string_to_integer<std::int64_t>(str.substr(0, delimiter_index)).value_or(0),
		conv::string_to_integer<std::int64_t>(str.substr(delimiter_index + 1)).value_or(0),
	};
}
}
