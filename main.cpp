#include <iostream>
#include <cmath>
#include <chrono>
#include <string>

using std::string;

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

void print_char_with_color(char c, RGB color) {
	std::cout << "\x1b[38;2;" << int(color.r*255) << ";" << int(color.g*255) << ";" << int(color.b*255) << "m" << c;
}

void usage(char *programName) {
	std::cout << "Usage: " << programName << " [text to rainbowize]\n";
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		usage(argv[0]);
		return 0;
	}

	std::srand(time(nullptr));
	long long time = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::srand(time);
	int randomHueOffset = rand() / double(RAND_MAX) * 360;

	string text = argv[1];
	int hue = randomHueOffset;
	for (char c : text) {
		RGB rgb = hsv_to_rgb(hue, 1, 1);
		print_char_with_color(c, rgb);
		hue += 10;
	}

	std::cout << "\x1b[0m";
}
