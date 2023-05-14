#ifndef PNGR_CLI_H_
#define PNGR_CLI_H_

#include "../lib/math.hh"

#include <string_view>
#include <getopt.h>


namespace cli
{
constexpr char const* help_message =
	"Usage:\n"
	"\tpngr (--help)\n"
	"\tpngr <path> --out <path> --filter <uint|[rgba]> --color <uint>\n"
	"\tpngr <path> --out <path> --slice  <uint,uint>   --color <uint (--thickness <uint>)\n"
	"\tpngr <path> --out <path> --draw   point         --color <uint> --start  <int,int>\n"
	"\tpngr <path> --out <path> --draw   line          --color <uint> --start  <int,int> --end <int,int> (--width <uint>) (--height <uint>)\n"
	"\tpngr <path> --out <path> --draw   circle        --color <uint> --start  <int,int> --end <int,int> (--fill  <uint>) (--thickness <uint>)\n"
	"\tpngr <path> --out <path> --draw   circle        --color <uint> --center <int,int> --radius <uint> (--fill  <uint>) (--thickness <uint>)\n"
	"\tpngr <path> --out <path> --draw   rect(angle)   --color <uint> --start  <int,int> --end <int,int> (--fill  <uint>) (--thickness <uint>) (--with-diags)\n"
	"\tpngr <path> --out <path> --draw   square        --color <uint> --start  <int,int> --side <uint>   (--fill  <uint>) (--thickness <uint>) (--with-diags)\n"
	"\nNote on usage:\n"
	"\t[...] - exactly one of surrounded tokens.\n"
	"\t(...) - optional.\n"
	"\nOptions:\n"
	"\tLong        \tShort \tDefault \tDescription\n"
	"\t--help      \t-h    \t        \t(flag) show this message\n"
	"\t--out       \t-o    \t        \toutput path\n"
	"\t--filter    \t-f    \t        \tcolor filter\n"
	"\t--draw      \t-d    \t        \tdraw shape\n"
	"\t--slice     \t-s    \t        \tsplit the image into NxM cells\n"
	"\t--color     \t-C    \t        \tprimary (stroke) color (may be hex)\n"
	"\t--fill      \t-F    \t        \trect,circle: secondary (fill) color (may be hex)\n"
	"\t--thickness \t-T    \t1       \tstroke thickness\n"
	"\t--width     \t-W    \t1       \tline: width\n"
	"\t--height    \t-H    \t1       \tline: height\n"
	"\t--radius    \t-R    \t1       \tcircle: radius\n"
	"\t--side      \t-S    \t0       \tsquare: side length\n"
	"\t--center    \t      \t0,0     \tcircle: center point\n"
	"\t--start     \t      \t0,0     \tline,rect,circle: start point\n"
	"\t--end       \t      \t0,0     \tline,rect,circle: end point\n"
	"\t--with-diags\t-D    \t        \trect,square: (flag) draw diagonals\n"
	"\nNote on options:\n"
	"\t(flag) - optional flag, doesn't have an argument.\n"
	"\tOptions with an integral argument may support hexadecimal numbers that must be prefixed with `0x`.\n"
	"\tAn option is considered required if and only if it is not a flag and no default value is specified for it.";

constexpr char const* invalid_usage_hint = "see --help for details on usage";

constexpr char const* point_delimiter = ",";

constexpr char const* truecolor_channels = "rgba";

constexpr std::size_t input_file_index = 1;

constexpr std::size_t min_number_of_arguments = input_file_index + 1;

constexpr std::size_t radius_default = 1;
constexpr std::size_t thickness_default = 1;
constexpr std::size_t width_default = 1;
constexpr std::size_t height_default = 1;

math::Vector const center_default;
math::Vector const start_default;
math::Vector const end_default;
math::Vector const slice_dimensions_default{1, 1};

constexpr char const* short_options = "ho:f:d:s:C:F:T:W:H:R:S:D";

enum ShortOption : char
{
	Help          = 'h',
	Out           = 'o',
	Filter        = 'f',
	Draw          = 'd',
	Slice         = 's',
	Color         = 'C',
	Fill          = 'F',
	Thickness     = 'T',
	Width         = 'W',
	Height        = 'H',
	Radius        = 'R',
	Side          = 'S',
	WithDiagonals = 'D',
};

enum class Shape
{
	None,
	Point,
	Line,
	Rectangle,
	Circle,
};

enum class Mode
{
	None,
	Draw,
	Filter,
	Slice,
};

option const options[]{
	{"help",       no_argument,       nullptr, ShortOption::Help},
	{"out",        required_argument, nullptr, ShortOption::Out},
	{"filter",     required_argument, nullptr, ShortOption::Filter},
	{"draw",       required_argument, nullptr, ShortOption::Draw},
	{"slice",      required_argument, nullptr, ShortOption::Slice},
	{"color",      required_argument, nullptr, ShortOption::Color},
	{"fill",       required_argument, nullptr, ShortOption::Fill},
	{"thickness",  required_argument, nullptr, ShortOption::Thickness},
	{"width",      required_argument, nullptr, ShortOption::Width},
	{"height",     required_argument, nullptr, ShortOption::Height},
	{"radius",     required_argument, nullptr, ShortOption::Radius},
	{"side",       required_argument, nullptr, ShortOption::Side},
	{"with-diags", no_argument,       nullptr, ShortOption::WithDiagonals},
	{"center",     required_argument, nullptr, 0},
	{"start",      required_argument, nullptr, 0},
	{"end",        required_argument, nullptr, 0},
	{nullptr,      0,                 nullptr, 0},
};

[[nodiscard]] extern bool is_hex(std::string_view const str) noexcept;
[[nodiscard]] extern math::Vector string_to_vector(std::string_view const str, std::string_view const delimiter);
}

#endif
