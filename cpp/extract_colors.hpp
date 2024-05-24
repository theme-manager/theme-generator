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

/**
 * @brief Get the rgb values from a given Hue value
 * 
 * @param hue the hue
 * @param value the value
 * @return std::tuple<int, int, int> 
 */
RGB getRgbFromHsv(HSV hsv) {
    // https://en.wikipedia.org/wiki/Hue#HSL_and_HSV
    float hue = std::get<0>(hsv);
    float s = std::get<1>(hsv);
    float v = std::get<2>(hsv);
    float r, g, b;
    
    float c = s * v;
    float x = c * (1 - std::abs(std::fmod(hue / 60.0f, 2) - 1));
    float m = v - c;
    if(hue < 60) {
        r = 1;
        g = hue / 60;
        b = 0;
    } else if(hue < 120) {
        r = 1 - (hue - 60) / 60;
        g = 1;
        b = 0;
    } else if(hue < 180) {
        r = 0;
        g = 1;
        b = (hue - 120) / 60;
    } else if(hue < 240) {
        r = 0;
        g = 1 - (hue - 180) / 60;
        b = 1;
    } else if(hue < 300) {
        r = (hue - 240) / 60;
        g = 0;
        b = 1;
    } else {
        r = 1;
        g = 0;
        b = 1 - (hue - 300) / 60;
    }
    r = (r + m) * c;
    g = (g + m) * c;
    b = (b + m) * c;

    return RGB { r * 255, g * 255, b * 255 };
}

HSV getHsvFromRgb(RGB rgb) {
    float r = std::get<0>(rgb) / 255.0f;
    float g = std::get<1>(rgb) / 255.0f;
    float b = std::get<2>(rgb) / 255.0f;

    float max = std::max(r, std::max(g, b));
    float min = std::min(r, std::min(g, b));
    float delta = max - min;

    float h = 0.0f, s = 0.0f;
    float v = max;

    if (delta < 0.00001f) {
        h = 0.0f;
        s = 0.0f;
        return HSV { h, s, v };
    }

    if (max > 0.0f) {
        s = delta / max;
    } else {
        s = 0.0f;
        h = 0.0f;
        return HSV { h, s, v };
    } 

    if (r >= max) {
        h = (g - b) / delta;
    } else if (g >= max) {
        h = 2.0f + (b - r) / delta;
    } else {
        h = 4.0f + (r - g) / delta;
    }

    h *= 60.0f;
    if (h < 0.0f) {
        h += 360.0f;
    }

    return HSV { h, s, v };
}

std::string getColorBar(float hue, int length, float hueValue = 1.0f) {
    //std::cout << "\t[hue:" << hue << "][length:" << length << "]\t";
    if(length <= 0) {
        length = 1;
    }

    int r, g, b;
    float increment = 100 / (float)length;
    std::string output = "";
    float i = 0;
    while(i < 100) {
        std::tuple rgb = getRgbFromHsv(HSV { hue, hueValue, 1.0f });
        int r = std::get<0>(rgb);
        int g = std::get<1>(rgb);
        int b = std::get<2>(rgb);
        output += getUnixColorCode(r, g, b) + "#";
        i+=increment;
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

float getPercentRange(std::vector<HsvCount>& map, int fromPercent, int toPercent, SORT_BY sorter) {
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
                sum += std::get<0>(map[i].first) * map[i].second;
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

HsvCount getAverageHue(std::vector<HsvCount>& hsvCount) {
    float sum = 0.0f;
    int count = 0;
    for(HsvCount hC : hsvCount) {
        sum += std::get<0>(hC.first) * hC.second;
        count += hC.second;
    }
    return HsvCount { HSV { sum / count, 1, 1}, count };
}

float getAverageValue(std::vector<HsvCount>& hsvCount) {
    float sum = 0.0f;
    int count = 0;
    for(HsvCount hC : hsvCount) {
        sum += std::get<2>(hC.first) * hC.second;
        count += hC.second;
    }
    return sum / count;
}

std::vector<HsvCount> getSortedHueCounts(HsvMap& map, int minValue, SORT_BY sorter) {
    std::vector<HsvCount> hueCount;

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