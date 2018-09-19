///**
// * Generate colored, upscaled, temporized map, with the Italian alert zones
// * and an Italy background, animated Gifs.
// */
//#include <vips/vips8>
//#include <gdal_priv.h>
//#include <filesystem>
//#include <ctime>
//#include <iostream>
//#include <iomanip>
//#include <gdal_utils.h>
//
//using namespace std::string_literals;
//namespace fs = std::filesystem;
//using namespace std::chrono;
//
//static const std::string DIR_FORMAT   = "%Y%m%d_%H";
//static const std::string DATE_FORMAT  = "%Y/%m/%d H%H UTF-0 ";
//static const std::string TEMP_PATH    = "/mnt/ramdisk/TEMP/";
//static const std::string PREVISTE     = "/cf_psm.tif";
//static const std::string BASE_PATH    = "/home/giovanni/Desktop/dati/";
//static const std::string COLORI       = "colors.txt";
//
///**
// * Converts UTC time string to a tm struct.
// * @param dateTime the string to convert
// * @return the string converted to tm
// */
//tm toTime(std::stringstream dateTime) {
//    struct tm newTm{};
//    dateTime >> std::get_time(&newTm, DIR_FORMAT.c_str());
//    return newTm;
//}
//
///**
// * Covert a pair of 4326 coordinates to 3857
// * @param x longitudes
// * @param y latitudes
// */
//void to3857(double *x, double *y) {
//    OGRSpatialReference sourceSRS, targetSRS;
//    sourceSRS.importFromEPSG(4326);
//    targetSRS.importFromEPSG(3857);
//    OGRCreateCoordinateTransformation(&sourceSRS, &targetSRS)->Transform(1, x, y);
//}
//
//int main(int argc, char *argv[]) {
//    //timing for bench
//    high_resolution_clock::time_point t1 = high_resolution_clock::now();
//    char dirName[12];
//    char timestamp[22];
//    //takes the args and convert them to tm struct
//    tm startDate = toTime(std::stringstream(argv[1]));
//    tm endDate = toTime(std::stringstream(argv[2]));
//    //calc the time differenze
//    int diffHours = (int) std::difftime(timegm(&endDate), timegm(&startDate)) / 3600;
//    //register gdal driver and create the datasets
//    GDALAllRegister();
//    GDALDataset *originalDataset;
//    GDALDataset *newDataset;
//    //option for the apply color to the tif file and set the alpha
//    char *optionForDEM[] = {const_cast<char *>("-alpha"), nullptr};
//    GDALDEMProcessingOptions *options = GDALDEMProcessingOptionsNew(optionForDEM, nullptr);
//
//
//
//
//
//    vips::VImage in = vips::VImage::new_from_file( argv[1],
//                                       vips::VImage::option()-> set( "access", VIPS_ACCESS_SEQUENTIAL ) );
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//    int g;
//    time_t date;
//    for (int i = 0; i < diffHours; ++i) {
//        //start of gdal processing block
//        date = timegm(&startDate);
//        strftime(dirName, 12, DIR_FORMAT.c_str(), gmtime(&date));
//        fs::create_directory(fs::path(TEMP_PATH + dirName));
//
//        originalDataset = (GDALDataset *) GDALOpen((BASE_PATH + dirName + PREVISTE).c_str(), GA_ReadOnly);
//        newDataset = (GDALDataset *) GDALDEMProcessing((TEMP_PATH + dirName + PREVISTE).c_str(),
//                                                       originalDataset,
//                                                       "color-relief",
//        COLORI.c_str(), options, &g);
//        GDALClose(newDataset); //write the processed tif to disk
//
//        startDate.tm_hour += 1;
//    }
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//    GDALClose(originalDataset);
//    GDALDEMProcessingOptionsFree(options);
//    high_resolution_clock::time_point t2 = high_resolution_clock::now();
//    auto duration = duration_cast<seconds>(t2 - t1).count();
//    std::cout << duration;
//    return 0;
//}