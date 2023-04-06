#ifndef PNGR_IMAGE_PNG_H_
#define PNGR_IMAGE_PNG_H_

#include "./image.hh"
#include <memory>
#include <png.h>


namespace image::png
{
constexpr uint64_t signature = 0x89504E470D0A1A0A;

enum ColorType : uint8_t
{
	GS = 0,
	RGB = 2,
	Indexed = 3,
	GSA = 4,
	RGBA = 6,
};

struct Metadata
{
	uint32_t width;
	uint32_t height;

	ColorType color_type;

	uint8_t compression_method;
	uint8_t filter_method;
	uint8_t interlace_method;
};

class PNG : public Image
{
	Metadata metadata{};

	std::unique_ptr<uint8_t*> rows;

	png_struct* cache;
	png_info* info;
	png_info* end_info;

	size_t number_of_passes;

	color::Value pixel_mask;
	size_t pixel_stride;
	size_t pixels_per_byte;

	std::istream& is;

public:
	explicit PNG(std::istream& is);
	~PNG();

	void open(std::istream& is) & override;

	[[nodiscard]] size_t width() const& noexcept override;
	[[nodiscard]] size_t height() const& noexcept override;

	[[nodiscard]] color::Value get(math::Vector const& position) const& noexcept override;
	void set(math::Vector const& position, color::Value const value) const& noexcept override;

	void save(std::ostream& os) const& override;
};
}

#endif
