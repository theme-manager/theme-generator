#include <string>
#include <vector>
#include <map>

#include "color_operations.hpp"

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

HsvMap getHueDistribution(Image& image) {
    HsvMap hueDistribution;

    for (int y = 0; y < image.height; y++)
    {
        for (int x = 0; x < image.width; x++)
        {
            // Normalize the rgb values
            float r = image.data[y][x].r;
            float g = image.data[y][x].g;
            float b = image.data[y][x].b;

            HSV hsv = getHsvFromRgb(RGB { r, g, b });

            // Check if the hsv value exists in the dictionary. If not, add it. If it does, increment the counter
            if (hueDistribution.count(hsv)) {
                hueDistribution[hsv]++;
            } else {
                hueDistribution[hsv] = 1;
            }
        }
    }

    return hueDistribution;
}

/**
 * @brief Prints all hues and counts from a HueMap (Dictionary where the key is the hue and the value is the count)
 * 
 * @param map the hue map which will be printed
 */
void printHueDistribution(HsvMap& map) {
    for (auto it = map.begin(); it != map.end(); it++) {
        std::cout << "Hue: " << std::get<0>(it->first) << "°, \t\tCount: " << it->second << "\t\tColor: " << std::endl;
    }
}

/**
 * @brief Prints the whole hue count array given as is. If it is sorted, it will be printed in its sorted state as well
 * 
 * @param hueCount the hue count array which will be printed
 * @param barWidth the width of the color bar which will be printed to the right
 */
void printSortedHueCounts(std::vector<HsvCount>& hsvCount, int barWidth) {
    for (int i = 0; i < hsvCount.size(); i++) {
        std::tuple<int, int, int> rgb = getRgbFromHsv(hsvCount.at(i).first);
        int r = std::get<0>(rgb);
        int g = std::get<1>(rgb);
        int b = std::get<2>(rgb);
        bool twoOver100 = (r >= 100 && g >= 100) || (r >= 100 && b >= 100) || (g >= 100 && b >= 100);
        std::cout << "Hue: " << std::get<0>(hsvCount.at(i).first) << "°, \t\tCount: " << hsvCount.at(i).second << 
        "\trgb(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) << ")" << (twoOver100 ? "\t" : "\t\t") <<
        "Color: " << getColorBar(std::get<0>(hsvCount.at(i).first), barWidth) << getUnixNoColor() << std::endl;
    }
}