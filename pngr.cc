#include "./cli/cli.hh"
#include "./lib/image/png.hh"
#include "./lib/image/drawer.hh"
#include <iostream>
#include <fstream>


[[noreturn]] static inline void graceful_exit() noexcept
{
	exit(0);
}

[[noreturn]] static inline void print_and_exit(char const* const str) noexcept
{
	std::cout << str << std::endl;
	graceful_exit();
}

[[noreturn]] static inline void print_error_and_exit(char const* const str) noexcept
{
	std::cerr << "error: " << str << '\n' << cli::invalid_usage_hint << std::endl;
	graceful_exit();
}

[[noreturn]] static inline void print_help_and_exit() noexcept
{
	print_and_exit(cli::help_message);
}

int main(int const argc, char** const argv)
{
	if (argc <= cli::min_number_of_arguments)
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

	const char* const filepath_in = argv[cli::input_file_index];
	if (!strlen(filepath_in) || filepath_in[0] == '-')
	{
		print_help_and_exit();
	}

	char const* filepath_out = nullptr;

	size_t radius = cli::radius_default;
	size_t thickness = cli::thickness_default;
	size_t width = cli::width_default;
	size_t height = cli::height_default;

	char const* error_message = nullptr;

	int option_index = cli::input_file_index;
	int opt;
	try
	{
		while ((opt = getopt_long(argc, argv, "ho:f:d:C:F:T:W:H:R:", cli::options, &option_index)) != -1)
		{
			switch (opt)
			{
			case 0:
			{
				if (mode != cli::Mode::Draw)
				{
					print_help_and_exit();
				}

				char const* const option_name = cli::options[option_index].name;

				math::Vector const position(cli::vector_from_string(optarg, cli::point_delimiter));

				if (!strcmp(option_name, "start"))
				{
					start = position;
					break;
				}

				if (!strcmp(option_name, "end"))
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

				if (!strcmp(option_name, "center"))
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

				size_t const length = strlen(optarg);

				if (length < 1)
				{
					print_help_and_exit();
				}

				if (length == 1)
				{
					channel = std::string("rgba").find(tolower(optarg[0]));
				}

				if (length > 1 || channel == std::string::npos)
				{
					channel = std::stoull(optarg);
				}

				break;
			}

			case cli::ShortOption::Draw:
				if (mode != cli::Mode::None)
				{
					print_help_and_exit();
				}

				mode = cli::Mode::Draw;

				if (!strcmp(optarg, "point"))
				{
					shape = cli::Shape::Point;
				}
				else
				if (!strcmp(optarg, "line"))
				{
					shape = cli::Shape::Line;
				}
				else
				if (!strcmp(optarg, "rect") || !strcmp(optarg, "rectangle"))
				{
					shape = cli::Shape::Rectangle;
				}
				else
				if (!strcmp(optarg, "circle"))
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
		error_message = "argument number out of bounds";
	}
	catch (std::invalid_argument const& e)
	{
		error_message = "argument is not a valid number";
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
			if (start != end || start != math::Vector{})
			{
				if (radius != 1)
				{
					print_error_and_exit("circle bounds (start, end) and radius cannot specified together");
				}

				if (!is_secondary_value_specified)
				{
					dw.circle(start, end, thickness, primary_value);
					break;
				}

				dw.circle_filled(start, end, thickness, primary_value, secondary_value);
				break;
			}

			if (!is_secondary_value_specified)
			{
				dw.circle(center, radius, width, primary_value);
				break;
			}

			dw.circle_filled(center, radius, width, primary_value, secondary_value);
			break;
		}
	}
	std::ofstream os(filepath_out, std::ios::out | std::ios::binary);
	img.save(os);

	return 0;
}
