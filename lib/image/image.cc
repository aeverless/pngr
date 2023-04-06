#include "./image.hh"


namespace image
{
[[nodiscard]] size_t Image::index(math::Vector const& position) const& noexcept
{
	return position.x + position.y * width();
}

[[nodiscard]] math::Vector const Image::coordinates(size_t const i) const& noexcept
{
	return math::Vector{i % width(), i / width()};
}

[[nodiscard]] size_t Image::bind_x(int64_t const x) const& noexcept
{
	return math::clamp(x, static_cast<int64_t>(0), static_cast<int64_t>(width()) - 1);
}

[[nodiscard]] size_t Image::bind_y(int64_t const y) const& noexcept
{
	return math::clamp(y, static_cast<int64_t>(0), static_cast<int64_t>(height()) - 1);
}

[[nodiscard]] math::Vector const Image::bind(math::Vector const& position) const& noexcept
{
	return math::Vector{bind_x(position.x), bind_y(position.y)};
}

void Image::set_channel(math::Vector const& position, color::ChannelIndex const channel, color::Value const value) const& noexcept
{
	color::Value const channel_mask = (static_cast<color::Value>(1) << bit_depth) - 1;
	size_t const offset = (number_of_channels - 1 - channel) * bit_depth;
	set(position, (get(position) & ~(channel_mask << offset)) | ((value & channel_mask) << offset));
}
}
