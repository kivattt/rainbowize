#include <iostream>
#include <cmath>
#include <chrono>
#include <string>

#include "flagparser.hpp"

using std::string;

// RGB 0 to 1
struct RGB {
	double r,g,b;
};

// https://stackoverflow.com/a/6930407
RGB hsv_to_rgb(double h, double s, double v){
	if (h > 360)
		h = fmod(h, 360);

    double      hh, p, q, t, ff;
    long        i;
    RGB out;

    if(s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = v;
        out.g = v;
        out.b = v;
        return out;
    }
    hh = h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = v;
        break;
    case 5:
    default:
        out.r = v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}

RGB hex_str_to_rgb(string hex) {
	if (hex == "") {
		std::cerr << "hex_str_to_rgb() received an empty string\n";
		exit(1);
	}

	if (hex[0] != '#') {
		std::cerr << "hex_str_to_rgb() received an invalid hex string\n";
		exit(1);
	}

	std::transform(hex.begin(), hex.end(), hex.begin(), [](unsigned char c){return std::tolower(c);});

	string hexLookup = "0123456789abcdef";
	RGB ret;
	ret.r = double(hexLookup.find(hex[1]) << 4 | hexLookup.find(hex[2])) / 256;
	ret.g = double(hexLookup.find(hex[3]) << 4 | hexLookup.find(hex[4])) / 256;
	ret.b = double(hexLookup.find(hex[6]) << 4 | hexLookup.find(hex[6])) / 256;
	return ret;
}

void set_fg_color(RGB color) {
	std::cout << "\x1b[38;2;" << int(color.r*255) << ";" << int(color.g*255) << ";" << int(color.b*255) << "m";
}

void set_bg_color(RGB color) {
	std::cout << "\x1b[48;2;" << int(color.r*255) << ";" << int(color.g*255) << ";" << int(color.b*255) << "m";
}

void usage(char *programName) {
	std::cout << "Usage: " << programName << " [OPTIONS] [text to rainbowize]\n";
	std::cout << "\n";
	std::cout << "  -f Rainbow the foreground\n";
	std::cout << "  -b Rainbow the background\n";
	std::cout << "  -n Don't output a trailing newline\n";
	std::cout << "  --foreground=\"#aabbcc\" Constant foreground color\n";
	std::cout << "  --background=\"#aabbcc\" Constant background color\n";
	std::cout << "  --seed=1234 Specify random seed\n";
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage(argv[0]);
		return 0;
	}

	flagparser::FlagList flags = flagparser::get_flags(argc, argv);
	bool colorForeground = std::find(flags.characterFlags.begin(), flags.characterFlags.end(), 'f') != flags.characterFlags.end();
	bool colorBackground = std::find(flags.characterFlags.begin(), flags.characterFlags.end(), 'b') != flags.characterFlags.end();
	bool newline         = std::find(flags.characterFlags.begin(), flags.characterFlags.end(), 'n') == flags.characterFlags.end();
	string constantForeground = "";
	if (flags.get_value_flag_position("foreground") != flags.valueFlags.end())
		constantForeground = flags.valueFlags[flags.get_value_flag_position("foreground") - flags.valueFlags.begin()].value;
	string constantBackground = "";
	if (flags.get_value_flag_position("background") != flags.valueFlags.end())
		constantBackground = flags.valueFlags[flags.get_value_flag_position("background") - flags.valueFlags.begin()].value;

	string seed = "";
	if (flags.get_value_flag_position("seed") != flags.valueFlags.end())
		seed = flags.valueFlags[flags.get_value_flag_position("seed") - flags.valueFlags.begin()].value;

	if (seed == "") {
		long long time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		std::srand(time);
	} else {
		std::srand(std::atoi(seed.c_str()));
	}

	int randomHueOffset = rand() / double(RAND_MAX) * 360;

	string text = flags.plainFlags[0];
	int hue = randomHueOffset;

	for (char c : text) {
		RGB rgb = hsv_to_rgb(hue, 1, 1);
		if (colorForeground)               set_fg_color(hsv_to_rgb(hue, 1, 1));
		else if (constantForeground != "") set_fg_color(hex_str_to_rgb(constantForeground));

		if (colorBackground)               set_bg_color(hsv_to_rgb(hue, 1, 1));
		else if (constantBackground != "") set_bg_color(hex_str_to_rgb(constantBackground));

		std::cout << c;
		hue += 10;
	}

	std::cout << "\x1b[0m";
	if (newline) std::cout << '\n';
}
