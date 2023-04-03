#ifndef PNGR_IMAGE_H_
#define PNGR_IMAGE_H_

#include "../math.hh"
#include "../color.hh"
#include <istream>
#include <ostream>


namespace image
{
class Image
{
protected:
	size_t number_of_pixels;
	size_t number_of_channels;

	size_t bit_depth;

public:
	virtual void open(std::istream& is) = 0;

	virtual size_t width() const noexcept = 0;
	virtual size_t height() const noexcept = 0;

	[[nodiscard]] size_t index(math::Vector const& position) const noexcept;
	[[nodiscard]] math::Vector const coordinates(size_t const i) const noexcept;

	[[nodiscard]] size_t bind_x(int64_t const x) const noexcept;
	[[nodiscard]] size_t bind_y(int64_t const y) const noexcept;
	[[nodiscard]] math::Vector const bind(math::Vector const& position) const noexcept;

	[[nodiscard]] virtual color::Value get(math::Vector const& position) const noexcept = 0;
	virtual void set(math::Vector const& position, color::Value const value) const noexcept = 0;

	void set_channel(math::Vector const& position, color::ChannelIndex const channel, color::Value const value) const noexcept;

	virtual void save(std::ostream& os) const = 0;
};
}

#endif
