#include <iostream>
#include <string>
#include <vector>

extern "C" {
    #define STB_IMAGE_IMPLEMENTATION
    #include "stb_image.h"
}

struct Image {
    std::vector<std::vector<Pixel>> data;
    int width;
    int height;
};

struct Pixel {
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

    private:
        float r;
        float g;
        float b;
        float a;
};

/**
 * @brief 
 * 
 * @param path 
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

            std::cout << "RGBA pixel @ (x=" << x << ", y=" << y << "): " 
                << static_cast<int>(data[index + 0]) << " "
                << static_cast<int>(data[index + 1]) << " "
                << static_cast<int>(data[index + 2]) << " "
                << static_cast<int>(data[index + 3]) << std::endl;
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
Image extract_colors(const std::string& path) {    
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