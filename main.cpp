#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <string>
#include <ImageMagick-7/Magick++.h>

/**
 * Generate colored, upscaled, temporized map, with the Italian alert zones
 * and an Italy background, animated Gifs.
 */

using namespace std;
namespace fs = std::filesystem;


/**
 * Converts UTC time string to a tm struct.
 * To change the folders name set the format below @line 26
 * for more information check this table @link http://www.cplusplus.com/reference/ctime/strftime/
 *
 * @param dateTime the string to convert
 */
tm toTime(std::stringstream dateTime) {
    struct tm tm{};
    dateTime >> get_time(&tm, "%Y%m%d_%H");
    return tm;
}

int main(int argc, char* argv[]) {

    tm startDate = toTime(std::stringstream(argv[1]));
    tm endDate = toTime(std::stringstream(argv[2]));




    return 0;
}