#include <ctime>
#include <time.h>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <ImageMagick-7/Magick++.h>
#include <gdal_priv.h>

/**
 * Generate colored, upscaled, temporized map, with the Italian alert zones
 * and an Italy background, animated Gifs.
 */


const char * DATE_FORMAT = "%Y%m%d_%H";


/**
 * Converts UTC time string to a tm struct.
 * To change the folders name set the format below @line 26
 * for more information check this table @link http://www.cplusplus.com/reference/ctime/strftime/
 *
 * @param dateTime the string to convert
 */
tm toTime(std::stringstream dateTime) {
    struct tm tm{};
    dateTime >> std::get_time(&tm, DATE_FORMAT);
    return tm;
}

int main(int argc, char* argv[]) {

    std::string base_path = "/home/giovanni/Desktop/dati";
    std::string dirName;
    tm startDate = toTime(std::stringstream(argv[1]));
    const tm endDate = toTime(std::stringstream(argv[2]));

    int diffHours = (int) std::difftime(mktime(&startDate), mktime(&endDate)) / 3600;

    GDALAllRegister();
    GDALDataset *originalDataset[diffHours];
    GDALDataset *newDataset[diffHours];

    for (int i = 0; i < diffHours; ++i) {

        strftime(strdup(dirName.c_str()), dirName.size(), DATE_FORMAT, &startDate);
        originalDataset[i] = (GDALDataset*) GDALOpen(base_path.append("/" + dirName).c_str(), GA_ReadOnly);

    }

    return 0;
}