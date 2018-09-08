#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

/**
 * Generate colored, upscaled, temporized map, with the Italian alert zones
 * and an Italy background, animated Gifs.
 */

// Converts UTC time string to a time_t value.
time_t toTime(istringstream dateTime) {
    struct tm tm{};
    dateTime >>get_time(&tm, "%Y%m%d_%H");
    return mktime(&tm); //non va bene perché usa la timezone di sistema
}

int main(int argc, char* argv[]) {

    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i];
    }

    std::time_t startDate = toTime(std::istringstream(argv[1]));
    std::time_t endDate = toTime(std::istringstream(argv[1]));



    std::cout << ctime(&startDate);

    return 0;
}