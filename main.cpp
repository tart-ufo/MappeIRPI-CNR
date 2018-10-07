#include <ctime>
#include <iomanip>
#include <gdal_priv.h>
#include <filesystem>
#include <gdal_utils.h>
#include <iostream>
#include <vips/vips8>
#include <pqxx/pqxx>
#include <thread>
#include <fstream>
#include "Configuration.h"


/**
 * Generate colored, upscaled, temporized map, with the Italian alert zones
 * and an Italy background, animated Gifs.
 */

using namespace std::string_literals;
namespace fs = std::filesystem;
using namespace std::chrono;
using namespace vips;


/**
 * Converts UTC time string to a tm struct.
 * @param dateTime the string to convert
 * @return the string converted to tm
 */
tm toTmStruct(std::stringstream dateTime, std::string DIR_FORMAT) {
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

static VImage
draw_overlay(VImage background, VImage overlay, int x, int y, std::vector<double> ink = {255}) {
    // cut out the part of the background we modify
    VImage tile = background.extract_area(x, y, overlay.width(), overlay.height());

    // Make a constant image the size of the overlay area containing the
    // ink
    VImage ink_image = overlay.new_from_image(ink);

    // use the overlay as a mask to blend smoothly between the background
    // and the ink
    tile = overlay.ifthenelse(ink, tile, VImage::option()->set("blend", TRUE));

    // and insert the modified tile back into the image
    return background.insert(tile, x, y);
}

/* Composite a small image into a much larger one.
 */
static VImage
composite_glyph(VImage background, VImage glyph, int x, int y) {
    // cut out the part of the background we modify
    VImage tile = background.extract_area(x, y, glyph.width(), glyph.height());

    // use the overlay as a mask to blend smoothly between the background
    // and the ink
    tile = tile.composite(glyph, VIPS_BLEND_MODE_OVER);

    // and insert the modified tile back into the image
    return background.insert(tile, x, y);
}


int main(int argc, char *argv[]) {
    //init vips and register gdal driver
    if (VIPS_INIT(argv[0]))
        vips_error_exit(NULL);
    GDALAllRegister();
    // Disable te libvips cache -- it won't help and will just burn memory.
    vips_cache_set_max(0);

    //load conf from the configuration file
    preferences conf(argv[3]);

    //takes the args and convert them to tm struct
    tm startDate = toTmStruct(std::stringstream(argv[1]), conf.getDIR_FORMAT());
    tm endDate = toTmStruct(std::stringstream(argv[2]), conf.getDIR_FORMAT());
    int diffHours = (int) std::difftime(timegm(&endDate), timegm(&startDate)) / 3600;

    GDALDataset *originalDataset;
    GDALDataset *tempDataset;
    //option for the apply color to the tif file and set the alpha
    char *optionForDEM[] = {const_cast<char *>("-alpha"), nullptr};
    GDALDEMProcessingOptions *options = GDALDEMProcessingOptionsNew(optionForDEM, nullptr);

    //read background and overlay
    VImage background = VImage::new_from_file(conf.getBACKGROUND());
    VImage overlay = VImage::new_from_file(conf.getOVERLAY());
    VImage marker = vips::VImage::new_from_file(conf.getMARKER());

    std::vector<std::string> mapNames;
    std::vector<std::string> colorFiles;

    for (int i = 9; i < argc; ++i) {
        std::string map(argv[i]);
        mapNames.push_back(map);
        // if the last character of the name is a number, to find the corresponding color file I have to delete it
        while (isdigit(*map.rbegin())) {
            map.pop_back();
        }
        map.append("-colors.txt");
        colorFiles.push_back(map);
    }

/**********************************************************************************************************************
 * Image manipulating block
 */
    std::string dayDir(argv[1]);
    dayDir.append(argv[2]);
    fs::create_directory(fs::path(conf.getTEMP_PATH() + dayDir));

    std::string currentDir;
    char timestampString[23];
    time_t dateWip;
    char dateString[12];
    int gdalReturnCode;
    std::string ffmpegCommand;

    for (int i = 0; i < mapNames.size(); ++i) {
        // reset the startDate for next day
        tm startDate = toTmStruct(std::stringstream(argv[1]), conf.getDIR_FORMAT());
        currentDir = conf.getTEMP_PATH() + dayDir + "/" + mapNames.at(i);
        fs::create_directory(fs::path(currentDir));

        for (int ora = 0; ora < diffHours; ++ora) {
            dateWip = timegm(&startDate);
            strftime(dateString, 12, conf.getDIR_FORMAT(), gmtime(&dateWip));

            try {
                /******************************************* gdal block *******************************************/
                originalDataset = (GDALDataset *) GDALOpen(
                        (conf.getBASE_PATH() + dateString + "/" + mapNames.at(i) + ".tif").c_str(), GA_ReadOnly);
                tempDataset = (GDALDataset *) GDALDEMProcessing(
                        (currentDir + "/" + std::to_string(ora) + mapNames.at(i) + ".tif").c_str(),
                        originalDataset, "color-relief",
                        colorFiles.at(i).c_str(), options, &gdalReturnCode);
                GDALClose(tempDataset); //write the processed gdalTif to disk

                /******************************************* libvips block ***************************************/
                VImage tif = vips::VImage::new_from_file(
                        (currentDir + "/" + std::to_string(ora) + mapNames.at(i) + ".tif").c_str(),
                        VImage::option()->set("access", "sequential"));
                tif = tif.resize(3.022222222, VImage::option()->set("kernel", VIPS_KERNEL_NEAREST));
                VImage frame = background.composite2(tif, VIPS_BLEND_MODE_OVER);
                frame = draw_overlay(frame, overlay, 0, 0, {0});

                /* we must make a unique string each time, or libvips will
                 * cache the result for us
                 */
                strftime(timestampString, 22, conf.getDATE_FORMAT(), gmtime(&dateWip));
                VImage text = VImage::text(timestampString,
                                           VImage::option()->set("font", "SFmono 35")->set("fontfile", conf.getFONT()));
                frame = draw_overlay(frame, text, 470, 10);

                /*
                 * temporarily commented out for a future implementation of the markers obtained from the batabase
                 */
//                    int marker_x = 100;
//                    int marker_y = 300;
//                    frame = composite_glyph(frame, marker,
//                                            marker_x - marker.width() / 2,
//                                            marker_y - marker.height());

                frame.write_to_file((currentDir + "/" + mapNames.at(i) + "VIPS_" + std::to_string(ora) + ".jpeg").c_str());
                remove((currentDir + "/" + std::to_string(ora) + mapNames.at(i) + ".tif").c_str());

            } catch (vips::VError &e) {
                std::cerr << "MANCA UN GIORNO o c'è un problema con gli input per libvips:\n";
                std::cout << e.what();
            }



            //update date for next iteration
            startDate.tm_hour += 1;
        }

    }


/**********************************************************************************************************************
* close vips, gdal and free memory
*/
    vips_shutdown();

    GDALClose(originalDataset);
    GDALDEMProcessingOptionsFree(options);
    return 0;
}