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
using namespace std::chrono;

static std::string DATE_FORMAT = "%Y%m%d_%H";
static std::string TEMP_PATH = "/home/giovanni/Desktop/TEMP/";
static std::string PREVISTE = "/cf_psm.tif";
static std::string BASE_PATH = "/home/giovanni/Desktop/dati/";
static std::string COLORS = "colors.txt";

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
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    char dirName[12];
    tm startDate = toTime(std::stringstream(argv[1]));
    tm endDate = toTime(std::stringstream(argv[2]));
    int diffHours = (int) std::difftime(timegm(&endDate), timegm(&startDate)) / 3600;

    Magick::InitializeMagick("");
    GDALAllRegister();
    GDALDataset *originalDataset;
    GDALDataset *newDataset;

    Magick::Image sfondo;
    Magick::Image za;
    sfondo.read("/home/giovanni/CLionProjects/MappeIRPI-CNR/sfondo.png");
    za.read("/home/giovanni/CLionProjects/MappeIRPI-CNR/ZA.mpc");
    Magick::Blob bSfondo;
    sfondo.write(&bSfondo);

    char *optionForDEM[] = {const_cast<char *>("-alpha"), nullptr};
    GDALDEMProcessingOptions *options = GDALDEMProcessingOptionsNew(optionForDEM, nullptr);


    int g;
    time_t date;
    for (int i = 0; i < diffHours; ++i) {
        date = timegm(&startDate);
        startDate.tm_hour += 1;
        strftime(dirName, 12, DATE_FORMAT.c_str(), gmtime(&date));
        fs::create_directory(fs::path(TEMP_PATH + dirName));

        originalDataset = (GDALDataset *) GDALOpen((BASE_PATH + dirName + PREVISTE).c_str(), GA_ReadOnly);
        newDataset = (GDALDataset *) GDALDEMProcessing((TEMP_PATH + dirName + PREVISTE).c_str(),
                                                       originalDataset,
                                                          "color-relief",
                                                          COLORS.c_str(), options, &g);
        GDALClose(newDataset);
        Magick::Image tif;
        tif.read(TEMP_PATH + dirName + PREVISTE);
//        tif.zoom("2218x2275");
        tif.scale(Magick::Geometry(1083, 1166));

        Magick::Image foto;
        foto.read(bSfondo);
        foto.composite(tif, 0, 0, Magick::OverCompositeOp);
        foto.composite(za, 0, 0, Magick::OverCompositeOp);
        foto.write(TEMP_PATH + dirName + "/ppn2.png");
    }

    GDALClose(originalDataset);
    GDALDEMProcessingOptionsFree(options);


    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(t2 - t1).count();
    std::cout << duration;

    return 0;
}