#include <vips/vips8>
#include <gdal_priv.h>
#include <filesystem>
#include <ctime>

/**
 * Generate colored, upscaled, temporized map, with the Italian alert zones
 * and an Italy background, animated Gifs.
 */

using namespace std::string_literals;
namespace fs = std::filesystem;
using namespace std::chrono;

static std::string DIR_FORMAT = "%Y%m%d_%H";
static std::string DATE_FORMAT = "%Y/%m/%d H%H UTF-0 ";
static std::string TEMP_PATH = "/mnt/ramdisk/TEMP/";
static std::string PREVISTE = "/cf_psm.tif";
static std::string BASE_PATH = "/home/giovanni/Desktop/dati/";
static std::string COLORI = "colors.txt";

/**
 * Converts UTC time string to a tm struct.
 * @param dateTime the string to convert
 * @return the string converted to tm
 */
tm toTime(std::stringstream dateTime) {
    struct tm newTm{};
    dateTime >> std::get_time(&newTm, DIR_FORMAT.c_str());
    return newTm;
}

/**
 * Covert a pair of 4326 coordinates to 3857
 * @param x longitudes
 * @param y latitudes
 */
void to3857(double *x, double *y) {
    OGRSpatialReference sourceSRS, targetSRS;
    sourceSRS.importFromEPSG(4326);
    targetSRS.importFromEPSG(3857);
    OGRCreateCoordinateTransformation(&sourceSRS, &targetSRS)->Transform(1, x, y);
}

int main(int argc, char *argv[]) {
    //timing for bench
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    char dirName[12];
    char timestamp[22];

    //takes the args and convert them to tm struct
    tm startDate = toTime(std::stringstream(argv[1]));
    tm endDate = toTime(std::stringstream(argv[2]));
    //calc the time differenze
    int diffHours = (int) std::difftime(timegm(&endDate), timegm(&startDate)) / 3600;
    //register gdal driver and create the datasets
    GDALAllRegister();
    GDALDataset *originalDataset;
    GDALDataset *newDataset;
    //option for the apply color to the tif file and set the alpha
    char *optionForDEM[] = {const_cast<char *>("-alpha"), nullptr};
    GDALDEMProcessingOptions *options = GDALDEMProcessingOptionsNew(optionForDEM, nullptr);

}