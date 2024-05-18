#include "types.hpp"

const int BLOCK_SIZE = 30;

/**
 * @brief 
 * 
 */
struct QuickSortPartition {
    long unsigned int size;
    std::vector<HueCount> hueCount;
    QuickSortPartition* left;
    QuickSortPartition* right;
};

/**
 * @brief 
 * 
 * @param hueCount 
 */
void sortHueCount(std::vector<HueCount>& hueCount) {
    // Use the Quick Sort algorithm to partially sort the array into value-stepped partitions
    // Then use the pigeonhole sort to sort each block
    QuickSortPartition partition = { hueCount.size(), hueCount, nullptr, nullptr };

    partition = quickSortUntil(partition);
}

/**
 * @brief 
 * 
 * @param partition 
 * @return QuickSortPartition 
 */
QuickSortPartition quickSortUntil(QuickSortPartition partition) { 
    // Get the last hue count value in the partition and write all valuese bigger than it to the right, and smaller to the left
    if (partition.size <= BLOCK_SIZE) {
        countingSort(partition.hueCount);
        return partition;
    }

    int size = partition.hueCount.size();
    int refSize = partition.hueCount.at(size - 1).second;
    QuickSortPartition left;
    QuickSortPartition right; 

    for ( int i = size - 2; i >= 0; i-- ) {
        if ( partition.hueCount.at(i).second > refSize ) {
            right.hueCount.push_back(partition.hueCount.at(i));
        } else {
            left.hueCount.push_back(partition.hueCount.at(i));
        }
    }
    left.size = partition.left->hueCount.size();
    right.size = partition.right->hueCount.size();

    left = quickSortUntil(left);
    right = quickSortUntil(right);
    partition.left = &(left);
    partition.right = &(right);
    return partition;
}

/**
 * @brief 
 * 
 * @param hueCount 
 */
void countingSort(std::vector<HueCount>& hueCount) {
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
    std::vector<std::pair<int, HueCount>> countArray (range);

    // Fill the count array
    for (int i = 0; i < hueCount.size(); i++) {
        countArray.at(hueCount.at(i).second - min).first++;
        countArray.at(hueCount.at(i).second - min).second = hueCount.at(i);
    }

    // Store the comulative sums
    for (int i = 1; i < countArray.size(); i++) {
        countArray.at(i).first += countArray.at(i - 1).first;
    }

    // output the sorted array
    for (int i = countArray.size() - 1; i >= 0; i--) {
        hueCount.at(countArray.at(hueCount.at(i).second - min).first - 1) = countArray.at(i).second;
        countArray.at(hueCount.at(i).second - min).first--;
    }
}