#include <bitset>
#include "types.hpp"

/**
 * @brief Goes through all HsvCount ojects inside the hsvMap and returns only the ones whose count is greater than the minValue
 * 
 * @param[in] hsvMap the origin array of HsvCounts
 * @param[in] minValue the minimum count needed for a HsvCount to be returned
 * @return std::vector<HsvCount> the filtered array
 */
std::vector<HsvCount> getHueCountArrayFromMap(const HsvMap& hsvMap, const int minValue) {
    std::vector<HsvCount> hsvCount;
    for (HsvCount map_i : hsvMap) {
        if (map_i.second > minValue) {
            hsvCount.push_back(HsvCount { map_i.first, map_i.second });
        }
    }
    return hsvCount;
}

/**
 * @brief Goes through the complete Array of HsvCounts and sorts it based on the second value of the HsvCount, the count.
 * 
 * @param[out] hsvCount the array which gets sorted. Passed by reference.
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

/**
 * @brief Converts a given floating point number to an integer which is the binary representation of this float. This is done to improve
 * the performance of the counting sort (and make it possible in the first place, because counting sort can only handle integers)
 * 
 * @param[in] hue the floating point number which will be converted
 * @return int the converted number
 */
int convertFloatToBinary(const float hue) {
    int output;
    std::bitset<sizeof(float) * CHAR_BIT> bits(output);
    return (int)bits.to_ulong();
}

/**
 * @brief Goes trough an array of HsvCount objects and converts the hue value to an binary integer and saves it in the HsvCount objects
 * 
 * @param[in] hueCount (readonly) the array of HsvCount objects
 * @return std::vector<HsvCount> the array of HsvCount objects with the hue value converted
 */
std::vector<HsvCount> convertHueToBinary(const std::vector<HsvCount>& hueCount) {
    std::vector<HsvCount> binaryHueCount (hueCount.size());
    for (int i = 0; i < hueCount.size(); i++) {
        binaryHueCount.at(i) = HsvCount { hueCount.at(i).first, convertFloatToBinary(std::get<0>(hueCount.at(i).first)) };
    }
    return binaryHueCount;
}

/**
 * @brief converts the hues to integers and sorts the array based on this integer. 
 * 
 * @param[in] hueCount (readonly) the array which gets sorted
 * @return std::vector<HsvCount> the sorted array
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