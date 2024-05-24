#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "sorter.hpp"

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


std::string getUnixColorCode(int r, int g, int b) {
    return "\e[38;2;" + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}

std::string getUnixNoColor() {
    return "\e[0m";
}

std::tuple<int, int, int> getRgbFromHue(float hue, float value = 1) {
    // https://en.wikipedia.org/wiki/Hue#HSL_and_HSV
    //float s = 1;
    //float v = value;
    float r, g, b;
    
    //float c = s * v;
    //float x = c * (1 - std::abs(std::fmod(hue / 60.0f, 2) - 1));
    //float m = v - c;
    //if(hue < 60) {
    //    r = c;
    //    g = x;
    //    b = 0;
    //} else if(hue < 120) {
    //    r = x;
    //    g = c;
    //    b = 0;
    //} else if(hue < 180) {
    //    r = 0;
    //    g = c;
    //    b = x;
    //} else if(hue < 240) {
    //    r = 0;
    //    g = x;
    //    b = c;
    //} else if(hue < 300) {
    //    r = x;
    //    g = 0;
    //    b = c;
    //} else {
    //    r = c;
    //    g = 0;
    //    b = x;
    //}
    //r = (r + m);
    //g = (g + m);
    //b = (b + m);

    if (hue < 0) {
        hue = 0;
    }
    if (hue > 360) {
        hue = 360;
    }
    if (hue < 60) {
        r = 1;
        g = hue / 60;
        b = 0;
    } else if (hue < 120) {
        r = 1 - (hue - 60) / 60;
        g = 1;
        b = 0;
    } else if (hue < 180) {
        r = 0;
        g = 1;
        b = (hue - 120) / 60;
    } else if (hue < 240) {
        r = 0;
        g = 1 - (hue - 180) / 60;
        b = 1;
    } else if (hue < 300) {
        r = (hue - 240) / 60;
        g = 0;
        b = 1;
    } else {
        r = 1;
        g = 0;
        b = 1 - (hue - 300) / 60;
    }

    return std::make_tuple(r * 255, g * 255, b * 255);
}

std::string getColorBar(float hue, int length) {
    int r, g, b;
    int increment = 100 / length;
    std::string output = "";
    for(int i = 0; i < 100; i+=increment) {
        std::tuple rgb = getRgbFromHue(hue, i / length);
        int r = std::get<0>(rgb);
        int g = std::get<1>(rgb);
        int b = std::get<2>(rgb);
        output += getUnixColorCode(r, g, b) + "#";
    }
    return output;
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
            if (max - min < 0.00001f)
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

            if (hueDistribution.count(hue)) {
                hueDistribution[hue]++;
            } else {
                hueDistribution[hue] = 1;
            }
        }
    }

    return hueDistribution;
}

float getPercentRange(std::vector<HueCount>& map, int fromPercent, int toPercent, SORT_BY sorter) {
    if(fromPercent > 100 || toPercent > 100 || fromPercent < 0 || toPercent < 0 || fromPercent > toPercent) {
        return 0.0f;
    }

    float sum = 0.0f;
    int rangePercent = toPercent - fromPercent;
    int range = (map.size() * rangePercent) / 100;
    int min = (map.size() * fromPercent) / 100;
    int max = (map.size() * toPercent) / 100;
    int countSum = 0;

    for(int i = min; i <= max; i++) {
        switch (sorter) {
            case SORT_BY::HUE:
                sum += map[i].first * map[i].second;
                countSum += map[i].second;
                break;
            case SORT_BY::COUNT:
                sum += map[i].second;
                break;
        }
    }
    switch(sorter) {
        case SORT_BY::HUE:
            return sum / countSum;
        case SORT_BY::COUNT:
            return sum;
        default:
            return 0.0f;
    }
}

float getAverageHue(std::vector<HueCount>& hueCount) {
    float sum = 0.0f;
    int count = 0;
    for(HueCount hC : hueCount) {
        sum += hC.first * hC.second;
        count += hC.second;
    }
    return sum / count;
}

std::vector<HueCount> getSortedHueCounts(HueMap& map, int minValue, SORT_BY sorter) {
    std::vector<HueCount> hueCount;

    // sort the array
    if(sorter == SORT_BY::COUNT) {
        hueCount = getHueCountArrayFromMap(map, minValue);
        countingSortByCount(hueCount);
    } else if(sorter == SORT_BY::HUE) {
        hueCount = countingSortByHue(map);
    }
    
    return hueCount;
}


/**
 * @brief Prints all hues and counts from a HueMap (Dictionary where the key is the hue and the value is the count)
 * 
 * @param map the hue map which will be printed
 */
void printHueDistribution(HueMap& map) {
    for (auto it = map.begin(); it != map.end(); it++) {
        std::cout << "Hue: " << it->first << "°, \t\tCount: " << it->second << "\t\tColor: " << std::endl;
    }
}

/**
 * @brief Prints the whole hue count array given as is. If it is sorted, it will be printed in its sorted state as well
 * 
 * @param hueCount the hue count array which will be printed
 */
void printSortedHueCounts(std::vector<HueCount>& hueCount) {
    for (int i = 0; i < hueCount.size(); i++) {
        std::tuple<int, int, int> rgb = getRgbFromHue(hueCount.at(i).first);
        int r = std::get<0>(rgb);
        int g = std::get<1>(rgb);
        int b = std::get<2>(rgb);
        bool twoOver100 = (r >= 100 && g >= 100) || (r >= 100 && b >= 100) || (g >= 100 && b >= 100);
        std::cout << "Hue: " << hueCount.at(i).first << "°, \t\tCount: " << hueCount.at(i).second << 
        "\trgb(" + std::to_string(r) + ", " + std::to_string(g) + ", " + std::to_string(b) << ")" << (twoOver100 ? "\t" : "\t\t") <<
        "Color: " << getColorBar(hueCount.at(i).first, 20) << getUnixNoColor() << std::endl;
    }
}