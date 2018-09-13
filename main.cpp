#include <ctime>
#include <iomanip>
#include <ImageMagick-7/Magick++.h>
#include <gdal_priv.h>
#include <filesystem>
#include <gdal_utils.h>

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
static std::string COLOR = "/colors.txt";

/**
 * Converts UTC time string to a tm struct.
 * @param dateTime the string to convert
 * @return the string converted to tm
 */
tm toTime(std::stringstream dateTime) {
    struct tm newTm{};
    dateTime >> std::get_time(&newTm, DATE_FORMAT.c_str());
    return newTm;
}

int main(int argc, char *argv[]) {
    char dirName[13];
    tm startDate = toTime(std::stringstream(argv[1]));
    tm endDate = toTime(std::stringstream(argv[2]));
    int diffHours = (int) std::difftime(timegm(&endDate), timegm(&startDate)) / 3600;

    GDALAllRegister();
    GDALDataset *originalDataset[diffHours];
    GDALDataset *newDataset[diffHours];

    GDALDriver *memDriver = GetGDALDriverManager()->GetDriverByName("MEM");
    GDALDriver *gtiffDriver = GetGDALDriverManager()->GetDriverByName("GTiff");

    char *cose[] = {const_cast<char *>("-alpha"), NULL};
    GDALDEMProcessingOptions *options = GDALDEMProcessingOptionsNew(cose, NULL);

    int g;
    time_t date;
    for (int i = 0; i < diffHours; ++i) {
        date = timegm(&startDate);
        strftime(dirName, 13, DATE_FORMAT.c_str(), gmtime(&date));
        fs::create_directory(fs::path(TEMP_PATH + dirName));

        originalDataset[i] = (GDALDataset *) GDALOpen((BASE_PATH + dirName + PREVISTE).c_str(), GA_ReadOnly);
        newDataset[i] = (GDALDataset *) GDALDEMProcessing((TEMP_PATH + dirName + PREVISTE).c_str(),
                                                          originalDataset[i],
                                                          "color-relief",
                                                          "/home/giovanni/Desktop/colors.txt", options, &g);

        startDate.tm_hour += 1;
        GDALClose(newDataset[i]);
        GDALClose(originalDataset[i]);

    }
    GDALDEMProcessingOptionsFree(options);
    return 0;
}