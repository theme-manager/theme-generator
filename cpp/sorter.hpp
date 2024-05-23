#include <bitset>
#include "types.hpp"

std::vector<HueCount> getHueCountArrayFromMap(HueMap& hueMap, int minValue) {
    std::vector<HueCount> hueCount;
    for (std::pair<float, int> map_i : hueMap) {
        if (map_i.second > minValue) {
            hueCount.push_back(HueCount { map_i.first, map_i.second });
        }
    }
    return hueCount;
}

/**
 * @brief 
 * 
 * @param hueCount 
 */
void countingSortByCount(std::vector<HueCount>& hueCount) {
    int min = INT_MAX;
    int max = INT_MIN;

    // Find min and max values
    for (int i = 0; i < hueCount.size(); i++) {
        if (hueCount.at(i).second < min) {
            min = hueCount.at(i).second;
        }
        if (hueCount.at(i).second > max) {
            max = hueCount.at(i).second;
        }
    }
    int range = max - min + 1;
    std::vector<int> countArray (range);

    // Fill the count array
    for (int i = 0; i < hueCount.size(); i++) {
        countArray.at(hueCount.at(i).second - min)++;
    }

    // Store the cumulative sums
    for (int i = 1; i < countArray.size(); i++) {
        countArray.at(i) += countArray.at(i - 1);
    }

    // Sort the array
    std::vector<HueCount> output (hueCount.size());
    for (int i = hueCount.size() - 1; i >= 0; i--) {
        output.at(countArray.at(hueCount.at(i).second - min) - 1) = hueCount.at(i);
        countArray.at(hueCount.at(i).second - min)--;
    }

    hueCount = output;
}

int convertFloatToBinary(float hue) {
    int output;
    std::bitset<sizeof(float) * CHAR_BIT> bits(output);
    return (int)bits.to_ulong();
}

std::vector<HueCount> convertHueToBinary(std::vector<HueCount>& hueCount) {
    std::vector<HueCount> binaryHueCount (hueCount.size());
    for (int i = 0; i < hueCount.size(); i++) {
        binaryHueCount.at(i) = HueCount { hueCount.at(i).first, convertFloatToBinary(hueCount.at(i).first) };
    }
    return binaryHueCount;
}

/**
 * @brief 
 * 
 * @param hueCount 
 */
std::vector<HueCount> countingSortByHue(HueMap& hueMap) {
    std::vector<HueCount> hueCount = getHueCountArrayFromMap(hueMap, 0);
    std::vector<HueCount> binaryHueCount = convertHueToBinary(hueCount);
    countingSortByCount(binaryHueCount);

    for(int i = 0; i < hueCount.size(); i++) {
        binaryHueCount.at(i).second = hueMap[binaryHueCount.at(i).first];
    }

    return binaryHueCount;
}