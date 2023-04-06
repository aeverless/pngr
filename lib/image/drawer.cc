#include "./drawer.hh"
#include <cmath>


namespace image
{
Drawer::Drawer(Image& image) : img(image) {}

void Drawer::point(
	math::Vector const& position,
	color::Value const value
) const& noexcept
{
	if (0 <= position.x && position.x < img.width() && 0 <= position.y && position.y < img.height())
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

	if (first.x != last.x && start.x == end.x || first.y != last.y && start.y == end.y)
	{
		return;
	}

	for (size_t i = img.index(start); i <= img.index(end); i++)
	{
		point(img.coordinates(i), value);
	}
}

void Drawer::line_horizontal(
	int64_t const y,
	int64_t const x_left,
	int64_t const x_right,
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
	int64_t const x,
	int64_t const y_top,
	int64_t const y_bottom,
	color::Value const value
) const& noexcept
{
	if (0 > x || x >= img.width())
	{
		return;
	}

	for (size_t y = img.bind_y(y_top); y <= img.bind_y(y_bottom); y++)
	{
		point(math::Vector{x, y}, value);
	}
}

void Drawer::line(
	math::Vector const& start,
	math::Vector const& end,
	color::Value const value,
	size_t const width,
	size_t const height
) const& noexcept
{
	int64_t const dy = end.y - start.y;
	int64_t const dx = end.x - start.x;

	if (dy < 0)
	{
		return;
	}

	size_t const y_start = img.bind_y(start.y);
	size_t const y_end = img.bind_y(end.y);

	if (y_start == y_end && start.y != end.y)
	{
		return;
	}

	int64_t const line_x_left = dx >= 0 ? start.x : end.x;
	int64_t const line_x_right = dx >= 0 ? end.x : start.x;

	auto bind_x = [&line_x_left, &line_x_right](int64_t const x)
	{
		return math::clamp(x, line_x_left, line_x_right);
	};

	double const step_width = static_cast<double>(dx) / (dy + 1);

	for (int64_t y = y_start; y <= y_end; y++)
	{
		int64_t const x_start =
			y == start.y ? start.x : bind_x(start.x + step_width * (y - start.y) - ((dx < 0) * (width - 1)));
		int64_t const x_end =
			y == end.y ? end.x : bind_x(x_start + step_width + ((dx >= 0) * (width - 1)));

		int64_t const x_left  = dx >= 0 ? x_start : x_end;
		int64_t const x_right = dx >= 0 ? x_end : x_start;

		for (size_t y_offset = 0; y_offset < height; y_offset++)
		{
			line_horizontal(y + y_offset, x_left, x_right, value);
		}
	}
}

void Drawer::solid(
	math::Vector const& start,
	math::Vector const& end,
	color::Value const value
) const& noexcept
{
	size_t const y_start = img.bind_y(start.y);
	size_t const y_end = img.bind_y(end.y);

	if (y_start == y_end && start.y != end.y)
	{
		return;
	}

	for (size_t y = y_start; y <= y_end; y++)
	{
		line_horizontal(y, start.x, end.x, value);
	}
}

void Drawer::rectangle(
	math::Vector const& start,
	math::Vector const& end,
	size_t const stroke_thickness,
	color::Value const stroke_value,
	bool const with_diagonals,
	size_t const diagonal_thickness
) const& noexcept
{
	size_t const y_start = img.bind_y(start.y);
	size_t const y_end = img.bind_y(end.y);

	if (y_start == y_end && start.y != end.y)
	{
		return;
	}

	for (size_t y = y_start; y <= y_end; y++)
	{
		if (start.y <= y && y < start.y + stroke_thickness || end.y < y + stroke_thickness && y <= end.y)
		{
			line_horizontal(y, start.x, end.x, stroke_value);
		}
		else
		{
			line_horizontal(y, start.x, start.x + stroke_thickness - 1, stroke_value);
			line_horizontal(y, end.x - stroke_thickness + 1, end.x, stroke_value);
		}
	}

	if (with_diagonals)
	{
		size_t const thickness = !diagonal_thickness ? stroke_thickness : diagonal_thickness;
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
	size_t const stroke_thickness,
	color::Value const stroke_value,
	color::Value const fill_value,
	bool const with_diagonals,
	size_t const diagonal_thickness
) const& noexcept
{
	math::Vector const solid_offset{stroke_thickness, stroke_thickness};
	solid(start + solid_offset, end - solid_offset, fill_value);
	rectangle(start, end, stroke_thickness, stroke_value, with_diagonals, diagonal_thickness);
}

void Drawer::square(
	math::Vector const& start,
	size_t const side_length,
	size_t const stroke_thickness,
	color::Value const stroke_value,
	bool const with_diagonals,
	size_t const diagonal_thickness
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
	size_t const side_length,
	size_t const stroke_thickness,
	color::Value const stroke_value,
	color::Value const fill_value,
	bool const with_diagonals,
	size_t const diagonal_thickness
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
	size_t const stroke_thickness,
	color::Value const stroke_value
) const& noexcept
{
	size_t const dx = end.x - start.x;
	size_t const dy = end.y - start.y;

	if (dx != dy || dx == 0 || dy == 0)
	{
		return;
	}

	if (stroke_thickness > 1)
	{
		math::Vector const offset{1, 1};
		circle(start + offset, end - offset, stroke_thickness - 1, stroke_value);
	}

	size_t const center_y = start.y + dy / 2;
	size_t const radius = dx / 2;

	auto x = [&center_y, &radius](size_t const y)
	{
		return radius - sqrt(radius * radius - (center_y - y) * (center_y - y));
	};

	size_t const y_start = img.bind_y(start.y);
	size_t const y_end = img.bind_y(end.y);

	if (y_start == y_end && start.y != end.y)
	{
		return;
	}

	for (size_t y = y_start; y <= y_end; y++)
	{
		size_t const x_offset = x(y);
		size_t const x_left = start.x + x_offset;
		size_t const x_right = end.x - x_left + start.x;

		size_t const length = x(y + (y == start.y ? 1 : y == end.y || y < center_y ? -1 : 1)) - x_offset;
		line_horizontal(y, x_left, x_left + length, stroke_value);
		line_horizontal(y, x_right - length, x_right, stroke_value);
	}
}

void Drawer::circle(
	math::Vector const& center,
	size_t const radius,
	size_t const stroke_thickness,
	color::Value const stroke_value
) const& noexcept
{
	math::Vector const offset{radius, radius};
	circle(center - offset, center + offset, stroke_thickness, stroke_value);
}

void Drawer::circle_filled(
	math::Vector const& start,
	math::Vector const& end,
	size_t const stroke_thickness,
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
	size_t const radius,
	size_t const stroke_thickness,
	color::Value const stroke_value,
	color::Value const fill_value
) const& noexcept
{
	math::Vector const offset{radius, radius};
	circle(center - offset, center + offset, -1, fill_value);
	circle(center, radius, stroke_thickness, stroke_value);
}

void Drawer::slice(
	size_t const row_count,
	size_t const column_count,
	size_t const thickness,
	color::Value const value
) const& noexcept
{
	double const dx = static_cast<double>(img.width()) / column_count;
	double const dy = static_cast<double>(img.height()) / row_count;

	for (size_t i = 1; i < row_count; i++)
	{
		line_horizontal(dy * i, 0, img.width() - 1, value);
	}

	for (size_t i = 1; i < column_count; i++)
	{
		line_vertical(dx * i, 0, img.height() - 1, value);
	}
}

void Drawer::color_filter(
	color::ChannelIndex const channel,
	color::Value const value
) const& noexcept
{
	for (size_t i = 0; i < img.width() * img.height(); i++)
	{
		img.set_channel(img.coordinates(i), channel, value);
	}
}
}
