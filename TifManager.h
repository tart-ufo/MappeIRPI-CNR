//
// Created by giovanni on 06/09/18.
//

#ifndef MAPPEIRPI_CNR_TIFMANAGER_H
#define MAPPEIRPI_CNR_TIFMANAGER_H
#include <gdal.h>
#include <gdal_utils.h>
#include <gdal_priv.h>
#include <cpl_conv.h>

class TifManager {
public:
    TifManager();
    void CopyTif();
    void TifManipulator();
    void TiftoPng();
};


#endif //MAPPEIRPI_CNR_TIFMANAGER_H
