//
// Created by giovanni on 06/10/18.
//

#include <fstream>
#include <iostream>
#include "Configuration.h"

preferences::preferences(std::string path) {
    std::string line;
    std::fstream configFile;
    configFile.open(path);
    /*
     * takes the values ​​from the configuration file a line yes and a no, for now according to this pre-established order
     */
    int c = 1;
    while (std::getline(configFile, line)) {
        if(c % 2 == 0){
            switch (c) {
                case 2:
                    TEMP_PATH = line;
                    break;
                case 4:
                    BASE_PATH = line;
                    break;
                case 6:
                    BACKGROUND = line;
                    break;
                case 8:
                    OVERLAY = line;
                    break;
                case 10:
                    MARKER = line;
                    break;
                case 12:
                    FONT = line;
                    break;
                case 14:
                    DIR_FORMAT = line;
                    break;
                case 16:
                    DATE_FORMAT = line;
            }
        }
        ++c;
    }
    configFile.close();
}

const char* preferences::getBACKGROUND() {
    return BACKGROUND.c_str();
}

std::string preferences::getBASE_PATH() {
    return BASE_PATH;
}

const char* preferences::getDATE_FORMAT() {
    return DATE_FORMAT.c_str();
}

const char* preferences::getFONT() {
    return FONT.c_str();
}

const char* preferences::getOVERLAY() {
    return OVERLAY.c_str();
}

const char* preferences::getDIR_FORMAT() {
    return DIR_FORMAT.c_str();
}

const char* preferences::getMARKER() {
    return MARKER.c_str();
}

const char* preferences::getTEMP_PATH() {
    return TEMP_PATH.c_str();
}
