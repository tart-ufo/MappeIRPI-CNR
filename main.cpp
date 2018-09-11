#include <ctime>
#include <iomanip>
#include <ImageMagick-7/Magick++.h>
#include <gdal_priv.h>
#include <filesystem>

/**
 * Generate colored, upscaled, temporized map, with the Italian alert zones
 * and an Italy background, animated Gifs.
 */

using namespace std::string_literals;
namespace fs = std::filesystem;

static std::string DATE_FORMAT = "%Y%m%d_%H";
static std::string TEMP_PATH = "/home/giovanni/Desktop/TEMP/";
static std::string PREVISTE = "/cf_psm.tif";
static std::string BASE_PATH = "/home/giovanni/Desktop/dati/";

/**
 * Converts UTC time string to a tm struct.
 * To change the folders name set the format below @line 26
 * for more information check this table @link http://www.cplusplus.com/reference/ctime/strftime/
 *
 * @param dateTime the string to convert
 */
tm toTime(std::stringstream dateTime) {
    struct tm tm{};
    dateTime >> std::get_time(&tm, DATE_FORMAT.c_str());
//    tm.tm_isdst = -1;
    return tm;
}

int main(int argc, char* argv[]) {

    char dirName[13];
    tm startDate = toTime(std::stringstream(argv[1]));
    tm endDate = toTime(std::stringstream(argv[2]));

    int diffHours = (int) std::difftime(mktime(&endDate), mktime(&startDate)) / 3600;

    GDALAllRegister();
    GDALDataset *originalDataset[diffHours];
    GDALDataset *newDataset[diffHours];
    GDALDriver* driver = GetGDALDriverManager()->GetDriverByName("GTiff");

    time_t date;
    for (int i = 0; i < diffHours; ++i) {
        date = mktime(&startDate);
        strftime(dirName, 13, DATE_FORMAT.c_str(), gmtime(&date));
        fs::create_directory(fs::path(TEMP_PATH + dirName));
        originalDataset[i] = (GDALDataset *) GDALOpen((BASE_PATH + dirName + PREVISTE).c_str(), GA_ReadOnly);
        newDataset[i] = driver->CreateCopy((TEMP_PATH + dirName + PREVISTE).c_str(), originalDataset[0], FALSE, nullptr,
                                           nullptr, nullptr);
        startDate.tm_hour += 1;
    }

    for (int i = 0; i < diffHours; ++i) {
        GDALClose(newDataset[i]);
    }


    return 0;
}