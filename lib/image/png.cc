#include "png.hh"
#include "../io.hh"

#include <memory>


namespace image::png
{
PNG::PNG(std::istream& is)
{
	open(is);
}

void PNG::open(std::istream& is) &
{
	is.seekg(0);

	std::uint64_t header;
	io::read_endian(is, header, arch::Endian::Big);

	if (header != signature)
	{
		throw std::runtime_error("invalid png signature");
	}

	read_cache = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!read_cache)
	{
		throw std::runtime_error("could not create read cache");
	}

	read_info = png_create_info_struct(read_cache);
	if (!read_info)
	{
		throw std::runtime_error("could not create read info");
	}

	read_info_end = png_create_info_struct(read_cache);
	if (!read_info_end)
	{
		throw std::runtime_error("could not finish creating read info");
	}

	if (setjmp(png_jmpbuf(read_cache)))
	{
		throw std::runtime_error("error while reading");
	}

	png_set_sig_bytes(read_cache, sizeof(header));

	png_set_read_fn(
		read_cache,
		&is,
		[] (png_struct* cache, std::uint8_t* data, std::size_t size)
		{
			reinterpret_cast<std::istream*>(png_get_io_ptr(cache))->read(reinterpret_cast<char*>(data), size);
		}
	);

	png_read_info(read_cache, read_info);

	metadata.width = png_get_image_width(read_cache, read_info);
	metadata.height = png_get_image_height(read_cache, read_info);

	metadata.color_type = static_cast<ColorType>(png_get_color_type(read_cache, read_info));
	switch (metadata.color_type)
	{
	case ColorType::Indexed:
		if (!png_get_PLTE(read_cache, read_info, &palette, &palette_size) || !palette_size)
		{
			throw std::runtime_error("could not read palette");
		}
	case ColorType::GS:
		number_of_channels = 1;
		break;
	case ColorType::GSA:
		number_of_channels = 2;
		break;
	case ColorType::RGB:
		number_of_channels = 3;
		break;
	case ColorType::RGBA:
		number_of_channels = 4;
		break;
	}

	bit_depth = png_get_bit_depth(read_cache, read_info);

	pixel_mask = (static_cast<color::Value>(1) << (number_of_channels * bit_depth)) - 1;
	pixel_stride = (number_of_channels * bit_depth + 7) / 8;
	pixels_per_byte = 8 / (bit_depth * number_of_channels);

	metadata.compression_method = png_get_compression_type(read_cache, read_info);
	metadata.filter_method = png_get_filter_type(read_cache, read_info);
	metadata.interlace_method = png_get_interlace_type(read_cache, read_info);

	number_of_passes = png_set_interlace_handling(read_cache);

	png_read_update_info(read_cache, read_info);

	rows = std::make_unique<std::uint8_t*[]>(metadata.height);
	for (std::size_t i = 0; i < metadata.height; i++)
	{
		rows.get()[i] = new std::uint8_t[png_get_rowbytes(read_cache, read_info)];
	}

	png_read_image(read_cache, rows.get());
	png_read_end(read_cache, read_info);
}

PNG::~PNG()
{
	for (std::size_t i = 0; i < metadata.height; i++)
	{
		delete[] rows.get()[i];
	}

	png_destroy_read_struct(&read_cache, &read_info, &read_info_end);
}

[[nodiscard]] std::size_t PNG::color_depth() const& noexcept
{
	if (metadata.color_type == ColorType::Indexed)
	{
		return palette_size;
	}

	return static_cast<std::size_t>(1) << (bit_depth * number_of_channels);
}

[[nodiscard]] std::size_t PNG::width() const& noexcept
{
	return metadata.width;
}

[[nodiscard]] std::size_t PNG::height() const& noexcept
{
	return metadata.height;
}

[[nodiscard]] color::Value PNG::get(math::Vector const& position) const& noexcept
{
	if (pixels_per_byte > 1)
	{
		return rows.get()[position.y][position.x / pixels_per_byte] & (pixel_mask << ((position.x % pixels_per_byte) * pixel_stride));
	}

	return
		memory::to_big_endian(
			*reinterpret_cast<color::Value*>(&rows.get()[position.y][position.x * pixel_stride]), pixel_stride
		) & pixel_mask;
}

void PNG::set(math::Vector const& position, color::Value const value) const& noexcept
{
	if (pixels_per_byte > 1)
	{
		std::size_t const offset = (position.x % pixels_per_byte) * pixel_stride;
		std::uint8_t& byte = rows.get()[position.y][position.x / pixels_per_byte];
		byte = (byte & ~(pixel_mask << offset)) | ((value & pixel_mask) << offset);

		return;
	}

	color::Value& bytes = *reinterpret_cast<color::Value*>(&rows.get()[position.y][position.x * pixel_stride]);
	bytes = (bytes & ~pixel_mask) | (memory::to_big_endian(value, number_of_channels) & pixel_mask);
}

void PNG::save(std::ostream& os) const&
{
	png_struct* write_cache = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!write_cache)
	{
		throw std::runtime_error("could not create write cache");
	}

	png_info* write_info = png_create_info_struct(write_cache);
	if (!write_info)
	{
		throw std::runtime_error("could not create write info struct");
	}

	if (setjmp(png_jmpbuf(write_cache)))
	{
		png_destroy_write_struct(&write_cache, &write_info);
		throw std::runtime_error("error while writing");
	}

	png_set_write_fn(
		write_cache,
		&os,
		[] (png_struct* write_cache, std::uint8_t* data, std::size_t size)
		{
			reinterpret_cast<std::ostream*>(png_get_io_ptr(write_cache))->write(reinterpret_cast<char*>(data), size);
		},
		[] (png_struct* write_cache)
		{
			reinterpret_cast<std::ostream*>(png_get_io_ptr(write_cache))->flush();
		}
	);

	png_set_IHDR(
		write_cache,
		write_info,
		metadata.width,
		metadata.height,
		bit_depth,
		metadata.color_type,
		metadata.interlace_method,
		metadata.compression_method,
		metadata.filter_method
	);

	if (metadata.color_type == ColorType::Indexed)
	{
		png_set_PLTE(write_cache, write_info, palette, 1 << bit_depth);
	}

	png_write_info(write_cache, write_info);
	png_write_image(write_cache, rows.get());
	png_write_end(write_cache, nullptr);

	png_destroy_write_struct(&write_cache, &write_info);
}
}
