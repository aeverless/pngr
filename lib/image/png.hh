#ifndef PNGR_IMAGE_PNG_H_
#define PNGR_IMAGE_PNG_H_

#include "image.hh"

#include <memory>
#include <png.h>


namespace image::png
{
constexpr std::uint64_t signature = 0x89504E470D0A1A0A;

enum ColorType : std::uint8_t
{
	GS = 0,
	RGB = 2,
	Indexed = 3,
	GSA = 4,
	RGBA = 6,
};

struct Metadata
{
	std::uint32_t width;
	std::uint32_t height;

	ColorType color_type;

	std::uint8_t compression_method;
	std::uint8_t filter_method;
	std::uint8_t interlace_method;
};

class PNG : public Image
{
	Metadata metadata{};

	std::unique_ptr<std::uint8_t*[]> rows;

	png_struct* read_cache;
	png_info* read_info;
	png_info* read_info_end;

	png_color* palette;
	int palette_size;

	std::size_t number_of_passes;

	color::Value pixel_mask;
	std::size_t pixel_stride;
	std::size_t pixels_per_byte;

public:
	explicit PNG() noexcept = default;
	explicit PNG(std::istream& is);

	~PNG();

	void open(std::istream& is) & override;

	[[nodiscard]] std::size_t color_depth() const& noexcept override;

	[[nodiscard]] std::size_t width() const& noexcept override;
	[[nodiscard]] std::size_t height() const& noexcept override;

	[[nodiscard]] color::Value get(math::Vector const& position) const& noexcept override;
	void set(math::Vector const& position, color::Value const value) const& noexcept override;

	void save(std::ostream& os) const& override;
};
}

#endif
