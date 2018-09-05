#include <gdal_priv.h>
#include <cpl_conv.h>
#include <iostream>
#include <gdal.h>

/**
 * Manipulate a GeoTiff image with a single grayscale band
 * to an blue-tinted one
 * @return WIP
 */

int main()  {

    //Register all known driver
    GDALAllRegister();

    //open test input
    const char *originalTif = "/originalDataset.tif";
    auto originalDataset  = (GDALDataset *) GDALOpen(originalTif, GA_Update);

    // set the color interpretation to CGI_BlueBand, an enum for blue band
    originalDataset->GetRasterBand(1)->SetColorInterpretation(GCI_BlueBand);

    // create a new color entry for the new table
    const GDALColorEntry blueColorEntry ={ 0, 157, 255, 0};

    // create a new color table with the blue color entry
    GDALColorTable blueColorTable(GDALPaletteInterp=GPI_RGB);
    blueColorTable.SetColorEntry(1, &blueColorEntry);

    // set the new color table to the original file
    originalDataset->GetRasterBand(1)->SetColorTable(blueColorTable());

    //close the file before exit
    GDALClose(originalDataset);

    return 0;
}