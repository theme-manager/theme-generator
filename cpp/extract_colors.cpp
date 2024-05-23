#include <iostream>
#include <string>
#include "extract_colors.hpp"

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
    HueMap hueDistribution = getHueDistribution(image);
    //printHueDistribution(hueDistribution);

    // Sort the hue distribution by how often a specific hue occurs
    std::vector<HueCount> sortedCounts = getSortedHueCounts(hueDistribution, 0, SORT_BY::COUNT);
    std::vector<HueCount> sortedHues = getSortedHueCounts(hueDistribution, 0, SORT_BY::HUE);

    std::cout << "Hue Count: " << sortedCounts.size() << std::endl;
    int increment = 5;
    std::cout << "Hues percentages in " << increment << "\% increments sorted by their count:" << std::endl;
    for(int i = 0; i < 100/increment; i++) {
        std::cout << "Hue from " << i * increment << "\% to " << i * increment + increment << "\%: " << 
            getHuePercentRange(sortedCounts,  i * increment, i * increment + increment) << "°" << std::endl;
    }

    std::cout << "\nHues percentages in " << increment << "\% increments sorted by their hues:" << std::endl;
    for(int i = 0; i < 100/increment; i++) {
        std::cout << "Hue from " << i * increment << "\% to " << i * increment + increment << "\%: " << 
            getHuePercentRange(sortedHues,  i * increment, i * increment + increment) << "°" << std::endl;
    }

    return 0;
}