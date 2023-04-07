#include "./cli/cli.hh"
#include "./lib/image/png.hh"
#include "./lib/image/drawer.hh"
#include <iostream>
#include <fstream>
#include <cstring>


[[noreturn]] static inline void graceful_exit() noexcept
{
	exit(0);
}

[[noreturn]] static inline void print_and_exit(std::string_view const str) noexcept
{
	std::cout << str << std::endl;
	graceful_exit();
}

[[noreturn]] static inline void print_error_and_exit(std::string_view const str) noexcept
{
	std::cerr << "error: " << str << '\n' << cli::invalid_usage_hint << std::endl;
	graceful_exit();
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

	bool is_primary_value_specified = false;
	color::Value primary_value;

	bool is_secondary_value_specified = false;
	color::Value secondary_value;

	cli::Shape shape = cli::Shape::None;

	math::Vector center(cli::center_default);
	math::Vector start(cli::start_default);
	math::Vector end(cli::end_default);

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

				if (std::size_t index = std::stoull(optarg); index >= std::strlen(cli::truecolor_channels))
				{
					print_error_and_exit("channel index out of bounds");
				}
				else
				{
					channel = index;
				}

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
				if (!std::strcmp(optarg, "circle"))
				{
					shape = cli::Shape::Circle;
				}
				else
				{
					print_help_and_exit();
				}

				break;

			case cli::ShortOption::Color:
				primary_value = std::stoull(optarg, 0, cli::is_hex(optarg) ? 16 : 10);
				is_primary_value_specified = true;
				break;

			case cli::ShortOption::Fill:
				if (shape != cli::Shape::Rectangle && shape != cli::Shape::Circle)
				{
					print_help_and_exit();
				}

				secondary_value = std::stoull(optarg, 0, cli::is_hex(optarg) ? 16 : 10);
				is_secondary_value_specified = true;
				break;

			case cli::ShortOption::Thickness:
				if (shape != cli::Shape::Rectangle && shape != cli::Shape::Circle)
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

	if (!is_primary_value_specified)
	{
		print_error_and_exit("no primary color specified");
	}

	std::ifstream is(filepath_in, std::ios::in | std::ios::binary);
	if (!is.good())
	{
		print_error_and_exit("could not open input file for read");
	}

	image::png::PNG img(is);

	image::Drawer dw(img);

	if (mode == cli::Mode::Filter)
	{
		dw.color_filter(channel, primary_value);
	}
	else
	{
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
				dw.rectangle(start, end, thickness, primary_value);
			}
			else
			{
				dw.rectangle_filled(start, end, thickness, primary_value, secondary_value);
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
	}

	std::ofstream os(filepath_out, std::ios::out | std::ios::binary);
	img.save(os);

	return 0;
}
