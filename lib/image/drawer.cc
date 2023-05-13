#include "drawer.hh"

#include <algorithm>
#include <cmath>


namespace image
{
Drawer::Drawer(Image& image) : img(image) {}

void Drawer::point(
	math::Vector const& position,
	color::Value const value
) const& noexcept
{
	if ((0 <= position.x && position.x < img.width()) && (0 <= position.y && position.y < img.height()))
	{
		img.set(position, value);
	}
}

void Drawer::fill(
	math::Vector const& first,
	math::Vector const& last,
	color::Value const value
) const& noexcept
{
	math::Vector const& start(img.bind(first));
	math::Vector const& end(img.bind(last));

	if ((first.x != last.x && start.x == end.x) || (first.y != last.y && start.y == end.y))
	{
		return;
	}

	for (std::size_t i = img.index(start); i <= img.index(end); i++)
	{
		point(img.coordinates(i), value);
	}
}

void Drawer::line_horizontal(
	std::int64_t const y,
	std::int64_t const x_left,
	std::int64_t const x_right,
	color::Value const value
) const& noexcept
{
	if (0 > y || y >= img.height())
	{
		return;
	}

	fill(math::Vector{x_left, y}, math::Vector{x_right, y}, value);
}

void Drawer::line_vertical(
	std::int64_t const x,
	std::int64_t const y_top,
	std::int64_t const y_bottom,
	color::Value const value
) const& noexcept
{
	if (0 > x || x >= img.width())
	{
		return;
	}

	for (std::size_t y = img.bind_y(y_top); y <= img.bind_y(y_bottom); y++)
	{
		point(math::Vector{x, y}, value);
	}
}

void Drawer::line(
	math::Vector const& start,
	math::Vector const& end,
	color::Value const value,
	std::size_t const width,
	std::size_t const height
) const& noexcept
{
	std::int64_t const dy = end.y - start.y;
	std::int64_t const dx = end.x - start.x;

	if (dy < 0)
	{
		return;
	}

	std::int64_t const y_start = img.bind_y(start.y);
	std::int64_t const y_end = img.bind_y(end.y);

	if (y_start == y_end && start.y != end.y)
	{
		return;
	}

	std::int64_t const line_x_left = dx >= 0 ? start.x : end.x;
	std::int64_t const line_x_right = dx >= 0 ? end.x : start.x;

	auto bind_x = [&line_x_left, &line_x_right](std::int64_t const x)
	{
		return std::clamp(x, line_x_left, line_x_right);
	};

	double const step_width = static_cast<double>(dx) / (dy + 1);

	for (std::int64_t y = y_start; y <= y_end; y++)
	{
		std::int64_t const x = start.x + step_width * (y - start.y);
		std::int64_t const x_start = bind_x(x - ((dx < 0) * (width - 1)));
		std::int64_t const x_end = bind_x(x + ((dx >= 0) * (width - 1)));

		for (std::size_t y_offset = 0; y_offset < height; y_offset++)
		{
			line_horizontal(y + y_offset, x_start, x_end, value);
		}
	}
}

void Drawer::solid(
	math::Vector const& start,
	math::Vector const& end,
	color::Value const value
) const& noexcept
{
	std::size_t const y_start = img.bind_y(start.y);
	std::size_t const y_end = img.bind_y(end.y);

	if (y_start == y_end && start.y != end.y)
	{
		return;
	}

	for (std::size_t y = y_start; y <= y_end; y++)
	{
		line_horizontal(y, start.x, end.x, value);
	}
}

void Drawer::rectangle(
	math::Vector const& start,
	math::Vector const& end,
	std::size_t const stroke_thickness,
	color::Value const stroke_value,
	bool const with_diagonals,
	std::size_t const diagonal_thickness
) const& noexcept
{
	std::int64_t const y_start = img.bind_y(start.y);
	std::int64_t const y_end = img.bind_y(end.y);

	if (y_start == y_end && start.y != end.y)
	{
		return;
	}

	std::int64_t const thickness = static_cast<std::int64_t>(stroke_thickness);
	for (std::int64_t y = y_start; y <= y_end; y++)
	{
		if ((start.y <= y && y < start.y + thickness) || (end.y < y + thickness && y <= end.y))
		{
			line_horizontal(y, start.x, end.x, stroke_value);
		}
		else
		{
			line_horizontal(y, start.x, start.x + thickness - 1, stroke_value);
			line_horizontal(y, end.x - thickness + 1, end.x, stroke_value);
		}
	}

	if (with_diagonals)
	{
		std::size_t const thickness = !diagonal_thickness ? stroke_thickness : diagonal_thickness;
		math::Vector const offset{thickness, thickness};

		line(
			start + offset,
			end - offset,
			stroke_value,
			thickness
		);

		line(
			math::Vector{end.x - offset.x, start.y + offset.y},
			math::Vector{start.x + offset.x, end.y - offset.y},
			stroke_value,
			thickness
		);
	}
}

void Drawer::rectangle_filled(
	math::Vector const& start,
	math::Vector const& end,
	std::size_t const stroke_thickness,
	color::Value const stroke_value,
	color::Value const fill_value,
	bool const with_diagonals,
	std::size_t const diagonal_thickness
) const& noexcept
{
	math::Vector const solid_offset{stroke_thickness, stroke_thickness};
	solid(start + solid_offset, end - solid_offset, fill_value);
	rectangle(start, end, stroke_thickness, stroke_value, with_diagonals, diagonal_thickness);
}

void Drawer::square(
	math::Vector const& start,
	std::size_t const side_length,
	std::size_t const stroke_thickness,
	color::Value const stroke_value,
	bool const with_diagonals,
	std::size_t const diagonal_thickness
) const& noexcept
{
	rectangle(
		start,
		start + math::Vector{side_length, side_length},
		stroke_thickness,
		stroke_value,
		with_diagonals,
		diagonal_thickness
	);
}

void Drawer::square_filled(
	math::Vector const& start,
	std::size_t const side_length,
	std::size_t const stroke_thickness,
	color::Value const stroke_value,
	color::Value const fill_value,
	bool const with_diagonals,
	std::size_t const diagonal_thickness
) const& noexcept
{
	rectangle_filled(
		start,
		start + math::Vector{side_length, side_length},
		stroke_thickness,
		stroke_value,
		fill_value,
		with_diagonals,
		diagonal_thickness
	);
}

void Drawer::circle(
	math::Vector const& start,
	math::Vector const& end,
	std::size_t const stroke_thickness,
	color::Value const stroke_value
) const& noexcept
{
	std::size_t const dx = end.x - start.x;
	std::size_t const dy = end.y - start.y;

	if (dx != dy || dx == 0 || dy == 0)
	{
		return;
	}

	if (stroke_thickness > 1)
	{
		math::Vector const offset{1, 1};
		circle(start + offset, end - offset, stroke_thickness - 1, stroke_value);
	}

	std::size_t const center_y = start.y + dy / 2;
	std::size_t const radius = dx / 2;

	auto x = [&center_y, &radius](std::size_t const y)
	{
		return radius - std::sqrt(radius * radius - (center_y - y) * (center_y - y));
	};

	std::size_t const y_start = img.bind_y(start.y);
	std::size_t const y_end = img.bind_y(end.y);

	if (y_start == y_end && start.y != end.y)
	{
		return;
	}

	for (std::size_t y = y_start; y <= y_end; y++)
	{
		std::size_t const x_offset = x(y);
		std::size_t const x_left = start.x + x_offset;
		std::size_t const x_right = end.x - x_left + start.x;

		std::size_t const length = x(y + (y == start.y ? 1 : y == end.y || y < center_y ? -1 : 1)) - x_offset;
		line_horizontal(y, x_left, x_left + length, stroke_value);
		line_horizontal(y, x_right - length, x_right, stroke_value);
	}
}

void Drawer::circle(
	math::Vector const& center,
	std::size_t const radius,
	std::size_t const stroke_thickness,
	color::Value const stroke_value
) const& noexcept
{
	math::Vector const offset{radius, radius};
	circle(center - offset, center + offset, stroke_thickness, stroke_value);
}

void Drawer::circle_filled(
	math::Vector const& start,
	math::Vector const& end,
	std::size_t const stroke_thickness,
	color::Value const stroke_value,
	color::Value const fill_value
) const& noexcept
{
	math::Vector const offset{stroke_thickness, stroke_thickness};
	circle(start + offset, end - offset, -1, fill_value);
	circle(start, end, stroke_thickness, stroke_value);
}

void Drawer::circle_filled(
	math::Vector const& center,
	std::size_t const radius,
	std::size_t const stroke_thickness,
	color::Value const stroke_value,
	color::Value const fill_value
) const& noexcept
{
	circle(center, radius - stroke_thickness, -1, fill_value);
	circle(center, radius, stroke_thickness, stroke_value);
}

void Drawer::slice(
	std::size_t const row_count,
	std::size_t const column_count,
	std::size_t const thickness,
	color::Value const value
) const& noexcept
{
	double const dx = static_cast<double>(img.width()) / column_count;
	double const dy = static_cast<double>(img.height()) / row_count;

	for (std::size_t i = 1; i < row_count; i++)
	{
		line_horizontal(dy * i, 0, img.width() - 1, value);
	}

	for (std::size_t i = 1; i < column_count; i++)
	{
		line_vertical(dx * i, 0, img.height() - 1, value);
	}
}

void Drawer::color_filter(
	color::ChannelIndex const channel,
	color::Value const value
) const& noexcept
{
	for (std::size_t i = 0; i < img.width() * img.height(); i++)
	{
		img.set_channel(img.coordinates(i), channel, value);
	}
}
}
