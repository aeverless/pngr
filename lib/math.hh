#ifndef PNGR_MATH_H_
#define PNGR_MATH_H_

#include <algorithm>
#include <cstdint>


namespace math
{
struct Vector
{
	int64_t x;
	int64_t y;

	template <typename X, typename Y>
	explicit Vector(X const& x, Y const& y) : x(static_cast<int64_t>(x)), y(static_cast<int64_t>(y)) {}

	Vector() : Vector(0, 0) {}

	[[nodiscard]] Vector operator-() const noexcept
	{
		return Vector{-x, -y};
	}

	[[nodiscard]] Vector operator+(Vector const& other) const noexcept
	{
		return Vector{x + other.x, y + other.y};
	}

	[[nodiscard]] Vector operator-(Vector const& other) const noexcept
	{
		return operator+(-other);
	}

	[[nodiscard]] bool operator==(Vector const& other) const noexcept
	{
		return x == other.x && y == other.y;
	}

	[[nodiscard]] bool operator!=(Vector const& other) const noexcept
	{
		return !operator==(other);
	}

	template <typename T>
	[[nodiscard]] Vector operator*(T const& value) const noexcept
	{
		return Vector{x * value, y * value};
	}
};

template <typename T>
[[nodiscard]] static inline constexpr T const& clamp(T const& value, T const& low, T const& high) noexcept
{
	return std::min(high, std::max(low, value));
}
}

#endif
