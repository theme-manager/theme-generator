#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "sorter.hpp"

/**
 * @brief Get the unix color code from given rgb values. The returned string can be used to change the text 
 * color of the text inside the terminal which comes after the code
 * 
 * @param[in] rgb the RGB tripel containing the rgb values (0-255)
 * @return std::string the color code which can be used in a string stream to color all text after it
 */
std::string getUnixColorCode(const RGB& rgb) {
    return "\e[38;2;" + std::to_string(std::get<0>(rgb)) + ";" + 
        std::to_string(std::get<1>(rgb)) + ";" + 
        std::to_string(std::get<2>(rgb)) + "m";
}

/**
 * @brief Get the color code for no color (this means the default terminal color). Used to reset the text color 
 * after a colored section
 * 
 * @return std::string the color code for the default terminal color
 */
std::string getUnixNoColor() {
    return "\e[0m";
}

/**
 * @brief Get the rgb values from a given HSV object
 * 
 * @param[in] hsv the hue, saturation and value inside a tripel of floats
 * @return RGB the rgb object containing the colors
 */
RGB getRgbFromHsv(const HSV& hsv) {
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

/**
 * @brief Get the HSV values from a given RGB object. The RGB object is an tripel of integers, the HSV 
 * is a tripel of floats.
 * 
 * @param[in] rgb the red, green and blue values inside a tripel of integers
 * @return HSV the hue, saturation and value inside a tripel of floats
 */
HSV getHsvFromRgb(const RGB& rgb) {
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

/**
 * @brief Creates a color bar in the color of the given hue and value. The length of the color bar is determined by its length attribute.
 * 
 * @param[in] hue the HSV hue of the color bar
 * @param[in] length the length of the bar
 * @param[in] value the HSV value of the color bar
 * @return std::string the created color bar
 */
std::string getColorBar(const float hue, int length, const float value = 1.0f) {
    if(length <= 0) {
        length = 1;
    }

    RGB rgb = getRgbFromHsv(HSV { hue, value, 1.0f });
    std::string output = getUnixColorCode(rgb);
    for(int i = 0; i < length; i++) {
        output += "#";
    }
    return output;
}

/**
 * @brief Sorts the given HsvMap based on SORT_BY and puts the results in a new array of HsvCounts. Only 
 * respects values which counts are greater or equal to the minValue when SORT_BY::COUNT is used.
 * 
 * @param[in] map (readonly) The dictionary of all HSV values and their counts, unsorted and chaotic
 * @param[in] sorter determines the variable after which the array will get sorted
 * @param[in] minValue the minimum count for included HsvCounts (only used when SORT_BY::COUNT is used, otherwise ignored)
 * @return std::vector<HsvCount> the sorted array
 */
std::vector<HsvCount> getSortedHueCounts(HsvMap& map, const SORT_BY sorter, const int minValue = 0) {
    std::vector<HsvCount> hueCount;
    switch(sorter) {
        case HUE:
            hueCount = countingSortByHue(map);
            break;
        case COUNT:
            hueCount = getHueCountArrayFromMap(map, minValue);
            countingSortByCount(hueCount);
            break;
    }
    return hueCount;
}

/**
 * @brief Takes a HsvCount array and extracts all HsvCounts within a range of percent representing the 
 * percent of the indexed array. Depending on the SORT_BY, the value is either the average hue of all the 
 * extracted HsvCounts or the sum of all counts.
 * 
 * @param[in] hsvCounts The array which gets examined
 * @param[in] fromPercent the percentage from which the range starts
 * @param[in] toPercent the percentage at which the range ends
 * @param[in] sorter determines which variable will be returned
 * @return float either the average hue or the sum of counts, depending on the @ sorter
 */
float getPercentRange(const std::vector<HsvCount>& hsvCounts, const int fromPercent, const int toPercent, const SORT_BY sorter) {
    if(fromPercent > 100 || toPercent > 100 || fromPercent < 0 || toPercent < 0 || fromPercent > toPercent) {
        return 0.0f;
    }

    float sum = 0.0f;
    int rangePercent = toPercent - fromPercent;
    int range = (hsvCounts.size() * rangePercent) / 100;
    int min = (hsvCounts.size() * fromPercent) / 100;
    int max = (hsvCounts.size() * toPercent) / 100;
    int countSum = 0;

    for(int i = min; i <= max; i++) {
        switch (sorter) {
            case SORT_BY::HUE:
                sum += std::get<0>(hsvCounts[i].first) * hsvCounts[i].second;
                countSum += hsvCounts[i].second;
                break;
            case SORT_BY::COUNT:
                sum += hsvCounts[i].second;
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

/**
 * @brief Goes through all HsvCounts in the given array and returns the average hue from all HsvCounts.
 * Also saves the sum of counts in the returned HsvCount
 * 
 * @param[in] hsvCounts (readonly) the array which will get examined
 * @return HsvCount 
 */
HsvCount getAverageHue(const std::vector<HsvCount>& hsvCounts) {
    float sum = 0.0f;
    int count = 0;
    for(HsvCount hC : hsvCounts) {
        sum += std::get<0>(hC.first) * hC.second;
        count += hC.second;
    }
    return HsvCount { HSV { sum / count, 1, 1}, count };
}

/**
 * @brief Goes trhough all HsvCounts in the give array and returns the average value from all HsvCounts.
 * 
 * @param[in] hsvCounts (readonly) the array which will get examined
 * @return float the average value among all HsvCounts
 */
float getAverageValue(const std::vector<HsvCount>& hsvCounts) {
    float sum = 0.0f;
    int count = 0;
    for(HsvCount hC : hsvCounts) {
        sum += std::get<2>(hC.first) * hC.second;
        count += hC.second;
    }
    return sum / count;
}