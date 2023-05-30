#include "cli/cli.hh"
#include "lib/image/png.hh"
#include "lib/image/drawer.hh"

#include <iostream>
#include <fstream>
#include <cstring>
#include <optional>


[[noreturn]] static inline void graceful_exit() noexcept
{
	std::exit(0);
}

template <typename... Args>
[[noreturn]] static inline void print_and_exit(Args const&... args) noexcept
{
	(std::cout << ... << args) << std::endl;
	graceful_exit();
}

template <typename... Args>
[[noreturn]] static inline void print_error_and_exit(Args const&... args) noexcept
{
	print_and_exit("error: ", args..., '\n', cli::invalid_usage_hint);
}

[[noreturn]] static inline void print_help_and_exit() noexcept
{
	print_and_exit(cli::help_message);
}

int main(int const argc, char* const argv[])
{
	if (static_cast<std::size_t>(argc) <= cli::min_number_of_arguments)
	{
		print_help_and_exit();
	}

	cli::Mode mode = cli::Mode::None;

	color::ChannelIndex channel;

	std::optional<color::Value> primary_value_opt;
	std::optional<color::Value> secondary_value_opt;

	bool with_diagonals = false;

	cli::Shape shape = cli::Shape::None;

	math::Vector center(cli::center_default);
	math::Vector start(cli::start_default);
	math::Vector end(cli::end_default);

	math::Vector slice_dimensions(cli::slice_dimensions_default);

	char const* const filepath_in = argv[cli::input_file_index];
	if (!std::strlen(filepath_in) || filepath_in[0] == '-')
	{
		print_help_and_exit();
	}

	char const* filepath_out = nullptr;

	std::size_t radius = cli::radius_default;
	std::size_t thickness = cli::thickness_default;
	std::size_t width = cli::width_default;
	std::size_t height = cli::height_default;

	char const* error_message = nullptr;

	try
	{
		int option_index = cli::input_file_index;
		int opt;

		while ((opt = getopt_long(argc, argv, cli::short_options, cli::options, &option_index)) != -1)
		{
			switch (opt)
			{
			case 0:
			{
				if (mode != cli::Mode::Draw)
				{
					print_help_and_exit();
				}

				math::Vector const position(cli::string_to_vector(optarg, cli::point_delimiter));

				char const* const option_name = cli::options[option_index].name;

				if (!std::strcmp(option_name, "start"))
				{
					start = position;
					break;
				}

				if (!std::strcmp(option_name, "end"))
				{
					if (shape == cli::Shape::Point)
					{
						print_help_and_exit();
					}

					end = position;
					break;
				}

				if (shape != cli::Shape::Circle)
				{
					print_help_and_exit();
				}

				if (!std::strcmp(option_name, "center"))
				{
					center = position;
					break;
				}

				break;
			}

			case cli::ShortOption::Out:
				filepath_out = optarg;
				break;

			case cli::ShortOption::Filter:
			{
				if (mode != cli::Mode::None)
				{
					print_help_and_exit();
				}

				mode = cli::Mode::Filter;

				std::size_t const length = std::strlen(optarg);

				if (length < 1)
				{
					print_help_and_exit();
				}

				if (length == 1)
				{
					if (char const* const ptr = std::strchr(cli::truecolor_channels, tolower(optarg[0])); ptr)
					{
						channel = ptr - cli::truecolor_channels;
						break;
					}
				}

				channel = std::stoull(optarg);
				break;
			}

			case cli::ShortOption::Draw:
				if (mode != cli::Mode::None)
				{
					print_help_and_exit();
				}

				mode = cli::Mode::Draw;

				if (!std::strcmp(optarg, "point"))
				{
					shape = cli::Shape::Point;
				}
				else
				if (!std::strcmp(optarg, "line"))
				{
					shape = cli::Shape::Line;
				}
				else
				if (!std::strcmp(optarg, "rect") || !std::strcmp(optarg, "rectangle"))
				{
					shape = cli::Shape::Rectangle;
				}
				else
				if (!std::strcmp(optarg, "square"))
				{
					shape = cli::Shape::Rectangle;
				}
				else
				if (!std::strcmp(optarg, "circle"))
				{
					shape = cli::Shape::Circle;
				}
				else
				{
					print_help_and_exit();
				}

				break;

			case cli::ShortOption::Slice:
				if (mode != cli::Mode::None)
				{
					print_help_and_exit();
				}

				mode = cli::Mode::Slice;
				slice_dimensions = cli::string_to_vector(optarg, cli::point_delimiter);
				break;

			case cli::ShortOption::Color:
				primary_value_opt = std::stoull(optarg, nullptr, cli::is_hex(optarg) ? 16 : 10);
				break;

			case cli::ShortOption::Fill:
				if (shape != cli::Shape::Rectangle && shape != cli::Shape::Circle)
				{
					print_help_and_exit();
				}

				secondary_value_opt = std::stoull(optarg, nullptr, cli::is_hex(optarg) ? 16 : 10);
				break;

			case cli::ShortOption::Thickness:
				if (mode != cli::Mode::Slice && shape != cli::Shape::Rectangle && shape != cli::Shape::Circle)
				{
					print_help_and_exit();
				}

				thickness = std::stoull(optarg);
				break;

			case cli::ShortOption::Radius:
				if (shape != cli::Shape::Circle)
				{
					print_help_and_exit();
				}

				radius = std::stoull(optarg);
				break;

			case cli::ShortOption::Side:
			{
				if (shape != cli::Shape::Rectangle)
				{
					print_help_and_exit();
				}

				std::size_t const side = std::stoull(optarg);
				end = start + math::Vector{side, side};
				break;
			}

			case cli::ShortOption::Width:
				if (shape != cli::Shape::Line)
				{
					print_help_and_exit();
				}

				width = std::stoull(optarg);
				break;

			case cli::ShortOption::Height:
				if (shape != cli::Shape::Line)
				{
					print_help_and_exit();
				}

				height = std::stoull(optarg);
				break;

			case cli::ShortOption::WithDiagonals:
				if (shape != cli::Shape::Rectangle)
				{
					print_help_and_exit();
				}

				with_diagonals = true;
				break;

			case cli::ShortOption::Help:
			case '?':
			default:
				print_help_and_exit();
				break;
			}
		}
	}
	catch (std::out_of_range const& e)
	{
		error_message = "argument out of bounds";
	}
	catch (std::invalid_argument const& e)
	{
		error_message = "invalid argument";
	}
	catch (std::exception const& e)
	{
		error_message = e.what();
	}

	if (error_message)
	{
		print_error_and_exit(error_message);
	}

	if (!filepath_out)
	{
		print_error_and_exit("no output file specified");
	}

	if (!primary_value_opt.has_value())
	{
		print_error_and_exit("no primary color specified");
	}

	color::Value const primary_value = primary_value_opt.value();

	bool const is_secondary_value_specified = secondary_value_opt.has_value();
	color::Value const secondary_value = secondary_value_opt.value_or(color::Value{});

	std::ifstream is(filepath_in, std::ios::in | std::ios::binary);
	if (!is.good())
	{
		print_error_and_exit("could not open input file for read");
	}

	image::png::PNG img;

	try
	{
		img.open(is);
	}
	catch (std::runtime_error const& e)
	{
		print_error_and_exit(e.what());
	}

	std::size_t channels = img.channels();
	if (mode == cli::Mode::Filter && channel >= channels)
	{
		print_error_and_exit("channel index exceeding maximum (", channels, ")");
	}

	std::size_t const color_depth = img.color_depth();
	if (primary_value >= color_depth || secondary_value >= color_depth)
	{
		print_error_and_exit("color value exceeding maximum (", color_depth, ")");
	}

	image::Drawer dw(img);

	switch (mode)
	{
	case cli::Mode::Filter:
		dw.color_filter(channel, primary_value);
		break;

	case cli::Mode::Slice:
		dw.slice(slice_dimensions.x, slice_dimensions.y, thickness, primary_value);
		break;

	case cli::Mode::Draw:
		switch (shape)
		{
		case cli::Shape::Point:
			dw.point(start, primary_value);
			break;

		case cli::Shape::Line:
			dw.line(start, end, primary_value, width, height);
			break;

		case cli::Shape::Rectangle:
			if (!is_secondary_value_specified)
			{
				dw.rectangle(start, end, thickness, primary_value, with_diagonals);
			}
			else
			{
				dw.rectangle_filled(start, end, thickness, primary_value, secondary_value, with_diagonals);
			}
			break;

		case cli::Shape::Circle:
			if (start != end || start != cli::start_default)
			{
				if (radius != cli::radius_default)
				{
					print_error_and_exit("circle bounds (start, end) and radius cannot specified together");
				}

				if (!is_secondary_value_specified)
				{
					dw.circle(start, end, thickness, primary_value);
				}
				else
				{
					dw.circle_filled(start, end, thickness, primary_value, secondary_value);
				}
				break;
			}

			if (!is_secondary_value_specified)
			{
				dw.circle(center, radius, thickness, primary_value);
			}
			else
			{
				dw.circle_filled(center, radius, thickness, primary_value, secondary_value);
			}

			break;

		case cli::Shape::None:
		default:
			break;
		}

		break;

	case cli::Mode::None:
	default:
		print_help_and_exit();
		break;
	}

	std::ofstream os(filepath_out, std::ios::out | std::ios::binary);

	try
	{
		img.save(os);
	}
	catch (std::exception const& e)
	{
		print_error_and_exit(e.what());
	}

	return 0;
}
