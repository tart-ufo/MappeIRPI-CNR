#include <ctime>
#include <iomanip>
#include <gdal_priv.h>
#include <filesystem>
#include <gdal_utils.h>
#include <iostream>
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
static std::string DATE_FORMAT = "%Y/%m/%d H%H UTF-0";
static std::string TEMP_PATH = "TEMP/";
static std::string PREVISTE = "/cf_psm.tif";
static std::string BASE_PATH = "/home/giovanni/Desktop/dati/";
static std::string COLORI = "/home/giovanni/CLionProjects/MappeIRPI-CNR/colors.txt";
static std::string BACKGROUND = "/home/giovanni/CLionProjects/MappeIRPI-CNR/background.vips";
static std::string OVERLAY = "/home/giovanni/CLionProjects/MappeIRPI-CNR/overlay.vips";
static std::string SFMONO = "/usr/share/fonts/OTF/SFMono-Bold.otf";

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

    //takes the args and convert them to tm struct
    tm startDate = toTime(std::stringstream(argv[1]));
    tm endDate = toTime(std::stringstream(argv[2]));
    //calc the time differenze
    int diffHours = (int) std::difftime(timegm(&endDate), timegm(&startDate)) / 3600;

    GDALDataset *originalDataset;
    GDALDataset *tempDataset;
    //option for the apply color to the tif file and set the alpha
    char *optionForDEM[] = {const_cast<char *>("-alpha"), nullptr};
    GDALDEMProcessingOptions *options = GDALDEMProcessingOptionsNew(optionForDEM, nullptr);

    //read background and overlay
    VImage background = VImage::new_from_file(BACKGROUND.c_str(),
                                              VImage::option()->set("access", VIPS_ACCESS_SEQUENTIAL));
    VImage overlay = VImage::new_from_file(OVERLAY.c_str(),
                                           VImage::option()->set("access", VIPS_ACCESS_SEQUENTIAL));

/**********************************************************************************************************************
 * Image manipulating block
 */
    time_t dateWip;
    char dirName[12];
    char timestampString[22];
    int gdalReturnCode;
    for (int j = 0; j < diffHours; ++j) {
        //update the date for the next iteration
        startDate.tm_hour += 1;
        dateWip = timegm(&startDate);

        /******************************************** gdal block ******************************************************/
        strftime(dirName, 12, DIR_FORMAT.c_str(), gmtime(&dateWip));
        originalDataset = (GDALDataset *) GDALOpen((BASE_PATH + dirName + PREVISTE).c_str(), GA_ReadOnly);
        tempDataset = (GDALDataset *) GDALDEMProcessing((TEMP_PATH + std::to_string(j) + "cf_psm.tif").c_str(),
                                                        originalDataset,
                                                        "color-relief",
                                                        COLORI.c_str(), options, &gdalReturnCode);
        GDALClose(tempDataset); //write the processed gdalTif to disk

        /******************************************* libvips block ****************************************************/
        //open new generated tif
        VImage gdalTif = VImage::new_from_file((TEMP_PATH + std::to_string(j) + "cf_psm.tif").c_str(),
                                               VImage::option()->set("access", VIPS_ACCESS_SEQUENTIAL));
        //resize the tif to 945x1015
        gdalTif = gdalTif.resize(3, VImage::option()->set("kernel", VIPS_KERNEL_LINEAR));
        //overlay the tif to the background
        gdalTif = background.composite(gdalTif, VIPS_BLEND_MODE_OVER);
        //overlay the "alert zones" to the tif
        gdalTif = gdalTif.composite(overlay, VIPS_BLEND_MODE_OVER);
        //format the string with proper date format
        strftime(timestampString, 22, DATE_FORMAT.c_str(), gmtime(&dateWip));
        //create new text image
        VImage testo = VImage::text(timestampString, VImage::option()->set("height", 25)
                                                                     ->set("width", 945)
                                                                     ->set("font", "SFmono")
                                                                     ->set("fontfile", SFMONO.c_str()));
        //overlay the text to the tif
        gdalTif = testo.composite(gdalTif, VIPS_BLEND_MODE_DEST_OVER);
        //write the image to disk
        gdalTif.write_to_file((TEMP_PATH + std::to_string(j) + "VIPS-cf_psm.jpg").c_str());
    }

/**********************************************************************************************************************
* close vips, gdal and free memory
*/
    vips_shutdown();
    GDALClose(originalDataset);
    GDALDEMProcessingOptionsFree(options);
    return 0;
}