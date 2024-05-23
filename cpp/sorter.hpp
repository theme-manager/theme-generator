#include "types.hpp"

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

/**
 * @brief 
 * 
 * @param hueCount 
 */
void countingSortByHue(std::vector<HueCount>& hueCount) {
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