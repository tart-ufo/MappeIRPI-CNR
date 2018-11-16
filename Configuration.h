//
// Created by giovanni on 06/10/18.
//

#ifndef MAPPEIRPI_PREFERENCES_H
#define MAPPEIRPI_PREFERENCES_H


#include <string>

class preferences {
private:
    std::string TEMP_PATH;
    std::string BASE_PATH;
    std::string BACKGROUND;
    std::string OVERLAY;
    std::string MARKER;
    std::string FONT;
    std::string DIR_FORMAT;
    std::string WORKING_DIR;
    std::string DATE_FORMAT;
public:
    preferences(std::string);

    const char* getTEMP_PATH();
    std::string getBASE_PATH();
    const char* getBACKGROUND();
    const char* getOVERLAY();
    const char* getMARKER();
    const char* getFONT();
    const char* getDIR_FORMAT();
    const char* getDATE_FORMAT();
    const char* getWORKING_DIR();

    const std::string &getWORKING_DIR() const;
};


#endif //MAPPEIRPI_PREFERENCES_H
