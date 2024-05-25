#include <string>
#include <vector>
#include <map>

#include "color_operations.hpp"

/**
 * @brief Check whether a file exists
 * 
 * @param path the path to the file
    path = $HOME/.cache/square_wallpaper.pngth 
 * @return true when the file exists
 * @return false when the file does not exist
 */
bool fileExists (const std::string& path) {
    if (FILE *file = fopen(path.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}

/**
 * @brief Converts a given stb_image image to an custom Image type existing of Pixels. 
 * 
 * @param data the stb_image's data
 * @param width the width of the image
 * @param height the height of the image
 * @return Image the converted image
 */
Image stb_imageToImage(const std::vector<unsigned char> data, const int width, const int height) {
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
 * @brief Loads an image from a given path into the image variable and saves the dimensions of the image in x and y.
 * 
 * @param[out] image the loaded image
 * @param[in] filename the path to the image
 * @param[out] x the width of the loaded image
 * @param[out] y the height of the loaded image
 * @return true when the image was successfully loaded
 * @return false when the image could not be loaded
 */
bool load_image(Image& image, const std::string& filename, int& x, int&y)
{
    int n;
    unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);
    if (data != nullptr) {
        image = stb_imageToImage(std::vector<unsigned char>(data, data + x * y * 4), x, y);
    }
    stbi_image_free(data);
    return (data != nullptr);
}

/**
 * @brief Tries to load an image from a given path
 * 
 * @param[in] path the path to the image
 * @return Image the loaded image if successful. Throws an error and returns an empty image if the image could not be loaded.
 */
Image loadImage(const std::string& path) {    
    int width, height;
    Image image;
    bool success = load_image(image, path, width, height);
    if (!success) {
        std::cout << "Error loading image\n";
        // return a null object
        return image;
    }
    image.width = width;
    image.height = height;
    return image;
}

/**
 * @brief Get a dictionary containing all HSV values from an image and the number of occurences
 * 
 * @param[in] image the image from which the HSV values will be extracted
 * @return HsvMap the created dictionary
 */
HsvMap getHueDistribution(Image& image) {
    HsvMap hueDistribution;
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            HSV hsv = getHsvFromRgb(image.data[y][x].rgb());

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
 * @param[in] map the hue map which will be printed
 */
void printHueDistribution(const HsvMap& map) {
    for (auto it = map.begin(); it != map.end(); it++) {
        std::cout << "Hue: " << std::get<0>(it->first) << "°, \t\tCount: " << it->second << "\t\tColor: " << std::endl;
    }
}

/**
 * @brief Prints the whole hue count array given as is. If it is sorted, it will be printed in its sorted state as well
 * 
 * @param[in] hueCount the hue count array which will be printed
 * @param[in] barWidth the width of the color bar which will be printed to the right
 */
void printSortedHueCounts(const std::vector<HsvCount>& hsvCount, const int barWidth) {
    for (int i = 0; i < hsvCount.size(); i++) {
        RGB rgb = getRgbFromHsv(hsvCount.at(i).first);
        int r = std::get<0>(rgb);
        int g = std::get<1>(rgb);
        int b = std::get<2>(rgb);
        bool twoOver100 = (r >= 100 && g >= 100) || (r >= 100 && b >= 100) || (g >= 100 && b >= 100);
        std::cout << "Hue: " << std::get<0>(hsvCount.at(i).first) << "°, \t\tCount: " << hsvCount.at(i).second << 
        "\trgb(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) << ")" << (twoOver100 ? "\t" : "\t\t") <<
        "Color: " << getColorBar(std::get<0>(hsvCount.at(i).first), barWidth) << getUnixNoColor() << std::endl;
    }
}