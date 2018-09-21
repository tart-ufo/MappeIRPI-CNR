#include <ctime>
#include <iomanip>
#include <gdal_priv.h>
#include <filesystem>
#include <gdal_utils.h>
#include <iostream>
#include <ImageMagick-7/Magick++.h>
#include <omp.h>

/**
 * Generate colored, upscaled, temporized map, with the Italian alert zones
 * and an Italy background, animated Gifs.
 */

using namespace std::string_literals;
namespace fs = std::filesystem;
using namespace std::chrono;

static std::string DIR_FORMAT = "%Y%m%d_%H";
static std::string DATE_FORMAT = "%Y/%m/%d H%H UTF-0 ";
static std::string TEMP_PATH = "TEMP/";
static std::string PREVISTE = "/cf_psm.tif";
static std::string BASE_PATH = "dati/";
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

    Magick::Image background;
    Magick::Image za;
    background.read(TEMP_PATH + "background.mpc");
    za.read(TEMP_PATH + "/za.mpc");

    time_t date;
    int g;

    startDate = toTime(std::stringstream(argv[1]));
    for (int j = 0; j < diffHours; ++j) {
        date = timegm(&startDate);
        strftime(dirName, 12, DIR_FORMAT.c_str(), gmtime(&date));
//        fs::create_directory(fs::path(TEMP_PATH + dirName));

        originalDataset = (GDALDataset *) GDALOpen((BASE_PATH + dirName + PREVISTE).c_str(), GA_ReadOnly);
        newDataset = (GDALDataset *) GDALDEMProcessing((TEMP_PATH + std::to_string(j) + "cf_psm.tif").c_str(),
                                                       originalDataset,
                                                       "color-relief",
                                                       COLORI.c_str(), options, &g);
        GDALClose(newDataset); //write the processed tif to disk
        startDate.tm_hour += 1;
    }

    startDate = toTime(std::stringstream(argv[1]));
#pragma omp ordered
    for (int i = 0; i < diffHours; ++i) {
        date = timegm(&startDate);
        //start of gdal processing block
        Magick::Image tif;
        tif.read(std::to_string(i) + "cf_psm.tif");
        tif.scale(Magick::Geometry(1082, 1166));
//        add the background and the za
//        i want to apply that to the final gif, not to every single photo
        tif.composite(background, 0, 0, Magick::DstOverCompositeOp);
        tif.composite(za, 0, 0, Magick::OverCompositeOp);
        //options for annotate the frame
        tif.font("/usr/share/fonts/OTF/SFMono-Bold.otf");
        tif.fillColor("White");
        tif.fontPointsize(37);

        strftime(timestamp, 22, DATE_FORMAT.c_str(), gmtime(&date));
        tif.annotate(timestamp, Magick::NorthEastGravity);
        tif.write(TEMP_PATH + std::to_string(i) + "cf_psm.tif");
        startDate.tm_hour += 1;
    }

    GDALClose(originalDataset);
    GDALDEMProcessingOptionsFree(options);


    return 0;
}