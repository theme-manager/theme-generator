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

float getHuePercentRange(std::vector<HueCount>& map, int fromPercent, int toPercent) {
    if(fromPercent > 100 || toPercent > 100 || fromPercent < 0 || toPercent < 0 || fromPercent > toPercent) {
        return 0.0f;
    }

    float sum = 0.0f;
    int rangePercent = toPercent - fromPercent;
    int range = (map.size() * rangePercent) / 100;
    int min = (map.size() * fromPercent) / 100;
    int max = (map.size() * toPercent) / 100;

    for(int i = min; i <= max; i++) {
        sum += map[i].first;
    }
    return sum / range;
}

std::vector<HueCount> getSortedHueCounts(HueMap& map, int minValue, SORT_BY sorter) {
    std::vector<HueCount> hueCount;
    // fill the array
    int hueCountSize = 0;
    for (std::pair<float, int> map_i : map) {
        if (map_i.second > minValue) {
            hueCount.push_back(HueCount { map_i.first, map_i.second });
            hueCountSize++;
        }
    }

    // sort the array
    if(sorter == SORT_BY::COUNT) {
        countingSortByCount(hueCount);
    } else if(sorter == SORT_BY::HUE) {
        countingSortByHue(hueCount);
    }
    return hueCount;
}


void printHueDistribution(HueMap& map) {
    for (auto it = map.begin(); it != map.end(); it++) {
        std::cout << "Hue: " << it->first << "°, \t\tCount: " << it->second << std::endl;
    }
}

void printSortedHueCounts(std::vector<HueCount>& hueCount) {
    for (int i = 0; i < hueCount.size(); i++) {
        std::cout << "Hue: " << hueCount.at(i).first << "°, \t\tCount: " << hueCount.at(i).second << std::endl;
    }
}