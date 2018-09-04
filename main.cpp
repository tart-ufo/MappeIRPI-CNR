#include <gdal_priv.h>
#include <cpl_conv.h>
#include <iostream>

int main()  {
        const char *input = "/home/giovanni/Desktop/expected.tif";
        const char *output = "/home/giovanni/Desktop/result.png";

        GDALAllRegister();

        GDALDataset  *oldDataSet;
        GDALDriver *outputPNG;
        outputPNG = GetGDALDriverManager()->GetDriverByName("PNG");

        oldDataSet  = (GDALDataset *) GDALOpen(input, GA_ReadOnly );

        std::cout << "ora va cazzo";

    return 0;
}