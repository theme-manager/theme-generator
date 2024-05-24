#include <bitset>
#include "types.hpp"

std::vector<HsvCount> getHueCountArrayFromMap(HsvMap& hsvMap, int minValue) {
    std::vector<HsvCount> hsvCount;
    for (std::pair<HSV, int> map_i : hsvMap) {
        if (map_i.second > minValue) {
            hsvCount.push_back(HsvCount { map_i.first, map_i.second });
        }
    }
    return hsvCount;
}

/**
 * @brief 
 * 
 * @param hsvCount 
 */
void countingSortByCount(std::vector<HsvCount>& hueCount) {
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
    std::vector<HsvCount> output (hueCount.size());
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

std::vector<HsvCount> convertHueToBinary(std::vector<HsvCount>& hueCount) {
    std::vector<HsvCount> binaryHueCount (hueCount.size());
    for (int i = 0; i < hueCount.size(); i++) {
        binaryHueCount.at(i) = HsvCount { hueCount.at(i).first, convertFloatToBinary(std::get<0>(hueCount.at(i).first)) };
    }
    return binaryHueCount;
}

/**
 * @brief 
 * 
 * @param hueCount 
 */
std::vector<HsvCount> countingSortByHue(HsvMap& hueMap) {
    std::vector<HsvCount> hueCount = getHueCountArrayFromMap(hueMap, 0);
    std::vector<HsvCount> binaryHueCount = convertHueToBinary(hueCount);
    countingSortByCount(binaryHueCount);

    for(int i = 0; i < hueCount.size(); i++) {
        binaryHueCount.at(i).second = hueMap[binaryHueCount.at(i).first];
    }

    return binaryHueCount;
}