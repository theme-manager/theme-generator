#include <map>
#include <string>
#include <vector>
#include <tuple>

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"
}

typedef std::map<float, int> HueMap;
typedef std::pair<float, int> HueCount;

struct Pixel {
public:
    float r;
    float g;
    float b;
    float a;

    Pixel() {
        r = 0;
        g = 0;
        b = 0;
        a = 0;
    }

    Pixel(float _r, float _g, float _b, float _a) {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    Pixel operator+(const Pixel& other) const
    {
        return Pixel { r + other.r, g + other.g, b + other.b, a + other.a };
    }

    Pixel operator/(float divisor) const
    {
        return Pixel { r / divisor, g / divisor, b / divisor, a / divisor };
    }

    Pixel operator-(const Pixel& other) const
    {
        return Pixel { r - other.r, g - other.g, b - other.b, a - other.a };
    }

    std::string toString() {
        return "(" + std::to_string((int)r) + " " + 
                    std::to_string((int)g) + " " + 
                    std::to_string((int)b) + " " + 
                    std::to_string((int)a) + ")";
    }
};

struct Image {
    std::vector<std::vector<Pixel>> data;
    int width;
    int height;
};