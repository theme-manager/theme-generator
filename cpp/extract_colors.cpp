#include <iostream>
#include <iomanip>
#include <string>

#include "extract_colors.hpp"

/**
 * @brief Prints the HsvCount array with a given percent increment. Averages result based on the increment range. 
 * @param[in] hsvCount the HsvCount array which will be printed
 * @param[in] increment the percent range of each print
 * @param[in] sortedBy the name of the sorted parameter. Should be "Gue" or "Count"
 * @param[in] barWidth the maximum width of the colored bar
 */
void printHueCountWithIncrement(const std::vector<HsvCount>& hsvCount, const int increment, const std::string sortedBy, const int barWidth) {
    std::cout << "\nHues percentages in " << increment << "\% increments sorted by their " << sortedBy << ":" << std::endl;
    for(int i = 0; i < 100/increment; i++) {
        float averageHue = getPercentRange(hsvCount,  i * increment, i * increment + increment, SORT_BY::HUE);
        RGB rgb = getRgbFromHsv(HSV { averageHue, 1, 1 });
        int r = std::get<0>(rgb);
        int g = std::get<1>(rgb);
        int b = std::get<2>(rgb);
        std::cout << "Hue from " << i * increment << "\% to " << i * increment + increment << "\%: " << 
            "\taverage Hues: " << averageHue << "° " <<
            "\tsum of Count: " << (int)getPercentRange(hsvCount,  i * increment, i * increment + increment, SORT_BY::COUNT) << 
            "\trgb(" << std::to_string(r) << ", " << std::to_string(g) << ", " << std::to_string(b) << ")" <<
            "\tColor: " << getColorBar(averageHue, barWidth) << getUnixNoColor() << std::endl;
    }
}

/**
 * @brief Prints the given HsvCount array in groups of a given size and displays the count of each group in a colored bar, 
 * which can be maximal as long as the barWidth.
 * 
 * @param[in] hsvCount The array which gets grouped and displayed
 * @param[in] groupSize The size of the group in percent
 * @param[in] barWidth The maximum width of the bar
 */
void printHueCountGroups(const std::vector<HsvCount>& hsvCount, int groupSize, const int barWidth) {
    if(groupSize <= 0) {
        std::cout << "Group size must be greater than 0" << std::endl;
        return;
    } else if (groupSize > 360) {
        groupSize = 360;
    }

    std::cout << "\nCount of Hues in groups of size " << groupSize << "°" << std::endl;
    int index = 0;
    int groupIndex = 0;
    int maxCount = 0;
    std::vector<std::vector<HsvCount>> groups;
    for(int i = 0; i < 360; i += groupSize) {
        int count = 0;
        std::vector<HsvCount> group;
        while(index < hsvCount.size() && std::get<0>(hsvCount.at(index).first) <= i + groupSize) {
            group.push_back(hsvCount.at(index));
            count += hsvCount.at(index).second;
            index++;
        }
        if(count > maxCount) {
            maxCount = count;
        }
        groups.push_back(group);
    }

    for(int i = 0; i < groups.size(); i++) {
        if(!groups.at(i).size() > 0) {
            continue;
        }
        HsvCount averageHue = getAverageHue(groups.at(i));
        float averageValue = getAverageValue(groups.at(i));
        std::cout << std::setw(12);
        std::cout << "Group " << i;
        std::cout << std::setw(17) << std::right;
        std::cout << "average: " + std::to_string(std::get<0>(averageHue.first)) + "°";
        std::cout <<std::setw(20) << std::right;
        std::cout << "count: " + std::to_string(averageHue.second);
        if(groups.at(i).size() > 0) {
            float scale = ((5.0f - std::min(5.0f, std::abs(std::logf((float)maxCount / (float)averageHue.second)))) / 5.0f);
            std::cout << std::setw(barWidth) << std::left;
            std::cout << getColorBar(std::get<0>(averageHue.first), (int)((barWidth * scale)), averageValue);
        }
        std::cout << getUnixNoColor() << std::endl;
    }
}

/**
 * @brief Loads an image and extracts all colors from it and does som math and simple algorithmics to determine 
 * the images colors.
 * 
 * @param[in] argc the count of the arguments
 * @param[in] argv the arguments values
 * @return int the exit code of the program
 */
int main(int argc, char* argv[])
{
    // Check if enough arguments are provided.
    if( argc < 4 ) {
        std::cout << "Not enough arguments" << std::endl;
        std::cout << "1st argument: image path" << std::endl;
        std::cout << "2nd argument: size of the groups (int)" << std::endl;
        std::cout << "3rd argument: bar width (int)" << std::endl;
        return 1;
    }

    // Check if the file exists
    if( !fileExists(argv[1]) ) {
        std::cout << "File " << argv[1] << " does not exist" << std::endl;
        return 1;
    }

    // Check if the second argument is correct and convert it
    int groupSize = atoi(argv[2]); //convertCharToInt(argv[2]);
    int barWidth = atoi(argv[3]);
    
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
    std::vector<HsvCount> sortedCounts = getSortedHueCounts(hueDistribution, SORT_BY::COUNT, 20);
    std::vector<HsvCount> sortedHues = getSortedHueCounts(hueDistribution, SORT_BY::HUE);

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

    printHueCountGroups(sortedHues, groupSize, barWidth);
    //printSortedHueCounts(sortedHues, 50);

    return 0;
}