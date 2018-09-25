#include <ctime>
#include <iomanip>
#include <gdal_priv.h>
#include <filesystem>
#include <gdal_utils.h>
#include <iostream>
#include <omp.h>
#include <vips/vips8>

/**
 * Generate colored, upscaled, temporized map, with the Italian alert zones
 * and an Italy background, animated Gifs.
 */

using namespace std::string_literals;
namespace fs = std::filesystem;
using namespace std::chrono;
using namespace vips;

static std::string DIR_FORMAT = "%Y%m%d_%H";
static std::string DATE_FORMAT = "%Y/%m/%d H%H UTF-0 ";
static std::string O_FORMAT = "%Y%m%d_%H.tif";
static std::string TEMP_PATH = "TEMP/";
static std::string PREVISTE = "/cf_psm.tif";
static std::string BASE_PATH = "/home/giovanni/Desktop/dati/";
static std::string COLORI = "/home/giovanni/CLionProjects/MappeIRPI-CNR/colors.txt";
static std::string SFONDO = "/home/giovanni/CLionProjects/MappeIRPI-CNR/sfondo.jpg";
static std::string ZA = "/home/giovanni/CLionProjects/MappeIRPI-CNR/ZA.png";

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

    if (VIPS_INIT(argv[0]))
        vips_error_exit(NULL);
    GDALAllRegister();

    char dirName[12];
    char timestampString[22];

    //takes the args and convert them to tm struct
    tm startDate = toTime(std::stringstream(argv[1]));
    tm endDate = toTime(std::stringstream(argv[2]));
    //calc the time differenze
    int diffHours = (int) std::difftime(timegm(&endDate), timegm(&startDate)) / 3600;
    //register gdal driver and create the datasets


    GDALDataset *originalDataset;
    GDALDataset *newDataset;
    //option for the apply color to the tif file and set the alpha
    char *optionForDEM[] = {const_cast<char *>("-alpha"), nullptr};
    GDALDEMProcessingOptions *options = GDALDEMProcessingOptionsNew(optionForDEM, nullptr);


    VImage sfondo = VImage::new_from_file("/home/giovanni/CLionProjects/MappeIRPI-CNR/cmake-build-debug/italy.vips",
                                          VImage::option()->set("access", VIPS_ACCESS_SEQUENTIAL));
    VImage za = VImage::new_from_file("/home/giovanni/CLionProjects/MappeIRPI-CNR/cmake-build-debug/za.vips",
                                      VImage::option()->set("access", VIPS_ACCESS_SEQUENTIAL));

/**********************************************************************************************************************/
    time_t date;
    int gdalReturnCode;
//#pragma omp for private(gdalReturnCode, date)
    for (int j = 0; j < diffHours; ++j) {
        date = timegm(&startDate);
        strftime(dirName, 12, DIR_FORMAT.c_str(), gmtime(&date));
//        fs::create_directory(fs::path(TEMP_PATH + dirName));

        originalDataset = (GDALDataset *) GDALOpen((BASE_PATH + dirName + PREVISTE).c_str(), GA_ReadOnly);
        newDataset = (GDALDataset *) GDALDEMProcessing((TEMP_PATH + std::to_string(j) + "cf_psm.tif").c_str(),
                                                       originalDataset,
                                                       "color-relief",
                                                       COLORI.c_str(), options, &gdalReturnCode);
        GDALClose(newDataset); //write the processed tif to disk
//        VImage frame = VImage::new_memory();
        VImage tif = VImage::new_from_file((TEMP_PATH + std::to_string(j) + "cf_psm.tif").c_str(),
                                           VImage::option()->set("access", VIPS_ACCESS_SEQUENTIAL));

        tif = tif.resize(3, VImage::option()->set("kernel", VIPS_KERNEL_LINEAR));
        tif = sfondo.composite(tif, VIPS_BLEND_MODE_OVER);
        tif = tif.composite(za, VIPS_BLEND_MODE_OVER);

        strftime(timestampString, 12, DATE_FORMAT.c_str(), gmtime(&date));

        tif.write_to_file((TEMP_PATH + std::to_string(j) + "EDITcf_psm.png").c_str());
        startDate.tm_hour += 1;
    }


/**********************************************************************************************************************
*   close vips, gdal and free memory
*/
    vips_shutdown();
    GDALClose(originalDataset);
    GDALDEMProcessingOptionsFree(options);
    return 0;
}