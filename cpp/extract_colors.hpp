#include <iostream>
#include <string>
#include <vector>
#include <map>

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"
}

typedef std::map<float, int> HueMap;
typedef std::tuple<float, int> HueCount;

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

/**
 * @brief 
 * 
 * @param pa
    path = $HOME/.cache/square_wallpaper.pngth 
 * @return true 
 * @return false 
 */
bool fileExists (const std::string& path) {
    if (FILE *file = fopen(path.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

Image stb_imageToImage(std::vector<unsigned char> data, int width, int height) {
    const size_t RGBA = 4;
    Image image;
    image.data.resize(height, std::vector<Pixel> ( width ) );
    image.width = width;
    image.height = height;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            size_t index = RGBA * (y * width + x);
            image.data[y][x] = Pixel((float)data[index + 0], 
                (float)data[index + 1], (float)data[index + 2], (float)data[index + 3]);

            // std::cout << "RGBA pixel @ (x=" << x << ", y=" << y << "): " 
            //     << static_cast<int>(data[index + 0]) << " "
            //     << static_cast<int>(data[index + 1]) << " "
            //     << static_cast<int>(data[index + 2]) << " "
            //     << static_cast<int>(data[index + 3]) << std::endl;
        }
    }
    return image;
}

/**
 * @brief 
 * 
 * @param image 
 * @param filename 
 * @param x 
 * @param y 
 * @return true 
 * @return false 
 */
bool load_image(Image& image, const std::string& filename, int& x, int&y)
{
    int n;
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);
    if (data != nullptr)
    {
        image = stb_imageToImage(std::vector<unsigned char>(data, data + x * y * 4), x, y);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

/**
 * @brief 
 * 
 * @param path 
 * @return unsigned* 
 */
Image loadImage(const std::string& path) {    
    int width, height;
    Image image;
    bool success = load_image(image, path, width, height);
    if (!success)
    {
        std::cout << "Error loading image\n";
        // return a null object
        return image;
    }

    return image;
}

HueMap getHueDistribution(Image& image) {
    HueMap hueDistribution;

    for (int y = 0; y < image.height; y++)
    {
        for (int x = 0; x < image.width; x++)
        {
            // Print the images rbga values
            //std::cout << image.data[y][x].toString() << "\t\t";

            // Normalize the rgb values
            float r = image.data[y][x].r / 255.0f;
            float g = image.data[y][x].g / 255.0f;
            float b = image.data[y][x].b / 255.0f;

            // Calculate the hue
            float max = std::max(r, std::max(g, b));
            float min = std::min(r, std::min(g, b));
            float hue;
            if (max == min)
            {
                hue = 0.0f;
            }
            else if (max == r)
            {
                hue = (g - b) / (max - min);
            }
            else if (max == g)
            {
                hue = 2.0f + (b - r) / (max - min);
            }
            else
            {
                hue = 4.0f + (r - g) / (max - min);
            }
            hue *= 60.0f;
            if (hue < 0.0f)
            {
                hue += 360.0f;
            }
            //std::cout << "HUE: " << hue << "°" << std::endl;

            if (hueDistribution.count(hue)) {
                hueDistribution[hue]++;
            } else {
                hueDistribution[hue] = 1;
            }
        }
    }

    return hueDistribution;
}

std::vector<HueCount> getSortedHueCounts(HueMap& map) {
    std::vector<HueCount> hueCount;
    // fill the array
    for (auto it = map.begin(); it != map.end(); it++) {
        if(it->second > 10)
            hueCount.push_back(HueCount(it->first, it->second));
    }

    // sort the array
    

    return hueCount;
}


void printHueDistribution(HueMap& map) {
    for (auto it = map.begin(); it != map.end(); it++) {
        std::cout << "Hue: " << it->first << "°, \t\tCount: " << it->second << std::endl;
    }
}