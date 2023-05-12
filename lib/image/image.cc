#include "./image.hh"

#include <algorithm>


namespace image
{
[[nodiscard]] std::size_t Image::index(math::Vector const& position) const& noexcept
{
	return position.x + position.y * width();
}

[[nodiscard]] math::Vector Image::coordinates(std::size_t const i) const& noexcept
{
	return math::Vector{i % width(), i / width()};
}

[[nodiscard]] std::size_t Image::bind_x(std::int64_t const x) const& noexcept
{
	return static_cast<size_t>(std::clamp(x, static_cast<std::int64_t>(0), static_cast<std::int64_t>(width()) - 1));
}

[[nodiscard]] std::size_t Image::bind_y(std::int64_t const y) const& noexcept
{
	return static_cast<size_t>(std::clamp(y, static_cast<std::int64_t>(0), static_cast<std::int64_t>(height()) - 1));
}

[[nodiscard]] math::Vector Image::bind(math::Vector const& position) const& noexcept
{
	return math::Vector{bind_x(position.x), bind_y(position.y)};
}

void Image::set_channel(math::Vector const& position, color::ChannelIndex const channel, color::Value const value) const& noexcept
{
	color::Value const channel_mask = (static_cast<color::Value>(1) << bit_depth) - 1;
	std::size_t const offset = (number_of_channels - 1 - channel) * bit_depth;
	set(position, (get(position) & ~(channel_mask << offset)) | ((value & channel_mask) << offset));
}
}
