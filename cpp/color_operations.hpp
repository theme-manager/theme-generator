#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "sorter.hpp"


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