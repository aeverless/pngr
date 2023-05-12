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
	std::size_t number_of_pixels;
	std::size_t number_of_channels;

	std::size_t bit_depth;

public:
	virtual void open(std::istream& is) & = 0;

	virtual std::size_t width() const& noexcept = 0;
	virtual std::size_t height() const& noexcept = 0;

	[[nodiscard]] std::size_t index(math::Vector const& position) const& noexcept;
	[[nodiscard]] math::Vector coordinates(std::size_t const i) const& noexcept;

	[[nodiscard]] std::size_t bind_x(std::int64_t const x) const& noexcept;
	[[nodiscard]] std::size_t bind_y(std::int64_t const y) const& noexcept;
	[[nodiscard]] math::Vector bind(math::Vector const& position) const& noexcept;

	[[nodiscard]] virtual color::Value get(math::Vector const& position) const& noexcept = 0;
	virtual void set(math::Vector const& position, color::Value const value) const& noexcept = 0;

	void set_channel(math::Vector const& position, color::ChannelIndex const channel, color::Value const value) const& noexcept;

	virtual void save(std::ostream& os) const& = 0;
};
}

#endif
