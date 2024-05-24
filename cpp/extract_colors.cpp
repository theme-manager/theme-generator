#include <iostream>
#include <string>
#include "extract_colors.hpp"

/**
 * @brief 
 * 
 * @param hueCount 
 * @param increment 
 * @param sortedBy 
 * @param barWidth 
 */
void printHueCountWithIncrement(std::vector<HsvCount>& hsvCount, int increment, std::string sortedBy, int barWidth) {
    std::cout << "\nHues percentages in " << increment << "\% increments sorted by their " << sortedBy << ":" << std::endl;
    for(int i = 0; i < 100/increment; i++) {
        float averageHue = getPercentRange(hsvCount,  i * increment, i * increment + increment, SORT_BY::HUE);
        std::tuple<int, int, int> rgb = getRgbFromHsv(HSV { averageHue, 1, 1 });
        int r = std::get<0>(rgb);
        int g = std::get<1>(rgb);
        int b = std::get<2>(rgb);
        std::cout << "Hue from " << i * increment << "\% to " << i * increment + increment << "\%: " << 
            "\taverage Hues: " << averageHue << "° " <<
            "\tsum of Count: " << getPercentRange(hsvCount,  i * increment, i * increment + increment, SORT_BY::COUNT) << 
            "\trgb(" << std::to_string(r) << ", " << std::to_string(g) << ", " << std::to_string(b) << ")" <<
            "\tColor: " << getColorBar(averageHue, barWidth) << getUnixNoColor() << std::endl;
    }
}

void printHueCountGroups(std::vector<HsvCount>& hsvCount, int groupSize) {
    std::cout << "\nCount of Hues in groups of size " << groupSize << "°" << std::endl;
    int index = 0;
    for(int i = 0; i < 360; i += groupSize) {
        std::vector<HsvCount> group;
        while(index < hsvCount.size() && std::get<0>(hsvCount.at(index).first) <= i + groupSize) {
            group.push_back(hsvCount.at(index));
            index++;
        }
        HsvCount averageHue = getAverageHue(group);
        std::cout << "\tHue from " << i << "° to " << i + groupSize << "°: \taverage: " << std::get<0>(averageHue.first) << "° \tcount: " << averageHue.second << std::endl;
    }
}

/**
 * @brief 
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char* argv[])
{
    // Check if enough arguments are provided.
    if( argc < 2 ) {
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }

    // Check if the file exists
    if( !fileExists(argv[1]) ) {
        std::cout << "File " << argv[1] << " does not exist" << std::endl;
        return 1;
    }
    
    Image image = loadImage(argv[1]);
    // check if image is null
    if(image.data.size() == 0 || image.data.at(0).size() == 0) {
        std::cout << "Error extracting colors" << std::endl;
        return 1;
    }

    // Get the hue distribution
    HsvMap hueDistribution = getHueDistribution(image);
    //printHueDistribution(hueDistribution);

    // Sort the hue distribution by how often a specific hue occurs
    std::vector<HsvCount> sortedCounts = getSortedHueCounts(hueDistribution, 20, SORT_BY::COUNT);
    std::vector<HsvCount> sortedHues = getSortedHueCounts(hueDistribution, 20, SORT_BY::HUE);

    std::cout << "Hue Count: " << sortedCounts.size() << std::endl;
    std::cout << "Average hue normalized by the colors count: " << std::get<0>(getAverageHue(sortedCounts).first) << "°" << std::endl;
    
    if(false) {
        int increment = 5;
        int input;
        std::cout << "\nPlease enter the increment percentage to display: ";
        std::cin >> input;
        if(input > 0 && input <= 100) increment = input;

        // Print the sorted vectors sorted by counts and hues
        printHueCountWithIncrement(sortedCounts, increment, "Counts", 35);
        printHueCountWithIncrement(sortedHues, increment, "Hues", 35);

        std::cout << std::endl;
    }

    printHueCountGroups(sortedHues, 5);
    //printSortedHueCounts(sortedHues, 50);

    //std::cout << "Hue 120° 10 Steps: " << getColorBar(77, 10) << getUnixNoColor() << std::endl;

    return 0;
}