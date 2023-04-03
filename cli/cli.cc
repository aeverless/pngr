#include "./cli.hh"
#include "../lib/math.hh"
#include <string>
#include <stdexcept>


namespace cli
{
[[nodiscard]] math::Vector const vector_from_string(char const* const str, char const* const delimiter)
{
	std::string const s(str);
	size_t const delimiter_index = s.find(delimiter);

	if (delimiter_index == std::string::npos)
	{
		throw std::runtime_error("no delimiter found in argument");
	}

	return math::Vector{
		std::stoll(s.substr(0, delimiter_index)),
		std::stoll(s.substr(delimiter_index + 1))
	};
}

[[nodiscard]] bool is_hex(char const* const value) noexcept
{
	if (strlen(value) <= 2 || strncmp(value, "0x", 2))
	{
		return false;
	}

	for (size_t i = 2; i < strlen(value); i++)
	{
		char const& c = tolower(value[i]);
		if (!isdigit(c) && ('a' > c || c > 'f'))
		{
			return false;
		}
	}

	return true;
}
}
