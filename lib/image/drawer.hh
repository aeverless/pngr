#ifndef PNGR_IMAGE_DRAWER_H_
#define PNGR_IMAGE_DRAWER_H_

#include "./image.hh"


namespace image
{
class Drawer
{
	Image& img;

public:
	explicit Drawer(Image& image);

	void point(
		math::Vector const& position,
		color::Value const value
	) const noexcept;

	void fill(
		math::Vector const& first,
		math::Vector const& last,
		color::Value const value
	) const noexcept;

	void line_horizontal(
		int64_t const y,
		int64_t const x_left,
		int64_t const x_right,
		color::Value const value
	) const noexcept;

	void line_vertical(
		int64_t const x,
		int64_t const y_top,
		int64_t const y_bottom,
		color::Value const value
	) const noexcept;

	void line(
		math::Vector const& start,
		math::Vector const& end,
		color::Value const value,
		size_t const width = 1,
		size_t const height = 1
	) const noexcept;

	void solid(
		math::Vector const& start,
		math::Vector const& end,
		color::Value const value
	) const noexcept;

	void rectangle(
		math::Vector const& start,
		math::Vector const& end,
		size_t const stroke_thickness,
		color::Value const stroke_value,
		bool const with_diagonals = false,
		size_t const diagonal_thickness = 0
	) const noexcept;

	void rectangle_filled(
		math::Vector const& start,
		math::Vector const& end,
		size_t const stroke_thickness,
		color::Value const stroke_value,
		color::Value const fill_value,
		bool const with_diagonals = false,
		size_t const diagonal_thickness = 0
	) const noexcept;

	void square(
		math::Vector const& start,
		size_t const side_length,
		size_t const stroke_thickness,
		color::Value const stroke_value,
		bool const with_diagonals = false,
		size_t const diagonal_thickness = 0
	) const noexcept;

	void square_filled(
		math::Vector const& start,
		size_t const side_length,
		size_t const stroke_thickness,
		color::Value const stroke_value,
		color::Value const fill_value,
		bool const with_diagonals = false,
		size_t const diagonal_thickness = 0
	) const noexcept;

	void circle(
		math::Vector const& start,
		math::Vector const& end,
		size_t const stroke_thickness,
		color::Value const stroke_value
	) const noexcept;

	void circle(
		math::Vector const& center,
		size_t const radius,
		size_t const stroke_thickness,
		color::Value const stroke_value
	) const noexcept;

	void circle_filled(
		math::Vector const& start,
		math::Vector const& end,
		size_t const stroke_thickness,
		color::Value const stroke_value,
		color::Value const fill_value
	) const noexcept;

	void circle_filled(
		math::Vector const& center,
		size_t const radius,
		size_t const stroke_thickness,
		color::Value const stroke_value,
		color::Value const fill_value
	) const noexcept;

	void slice(
		size_t const row_count,
		size_t const column_count,
		size_t const thickness,
		color::Value const value
	) const noexcept;

	void color_filter(
		color::ChannelIndex const channel,
		color::Value const value
	) const noexcept;
};
}

#endif
