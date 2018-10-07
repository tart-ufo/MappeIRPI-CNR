//
// Created by giovanni on 27/09/18.
//

#include <cstring>
#include "ArgvManager.h"

ArgvManager::ArgvManager(char **argv) {
    argData.resize(9);
    fileName.resize(9);
    checked.resize(9);
    cumolative.resize(9);
    this->argSpeed = std::stoi(argv[4]);
    argDataParser(argv);
}

bool ArgvManager::isChecked(int pos) {
    printf("checked\n");
    printf("pos: %d\n", pos);
    printf("size: %d\n\n", colors.size());

    return checked.at(pos);
}

std::string ArgvManager::getCumolative(int pos) {
    printf("cumolative\n");
    printf("pos: %d\n", pos);
    printf("size: %d\n\n", colors.size());

    return cumolative.at(pos);
}

std::string ArgvManager::getFilename(int pos) {
    printf("file name\n");
    printf("pos: %d\n", pos);
    printf("size: %d\n\n", colors.size());

    return fileName.at(pos);
}

std::string ArgvManager::getColor(int pos) {
    printf("color\n");
    printf("pos: %d\n", pos);
    printf("size: %d\n\n", colors.size());

    return colors.at(pos);
}

void ArgvManager::argDataParser(char *argv[]) {

    for (int i = 6; i <= 14; ++i) {
        if (strcmp(argv[i], "0") != 0) {
            checked.push_back(true);
            if (strcmp(argv[i], "T") == 0) {
                cumolative.push_back("");
            } else {
                cumolative.push_back(argv[i]);
            }

            switch (i) {
                case 0:
                    fileName.push_back(PROBABILITA_COMBINATA);
                    colors.push_back(COLORS);
                    break;
                case 1:
                    fileName.push_back(PROBABILITA_PREVISTA);
                    colors.push_back(COLORS);
                    break;
                case 2:
                    fileName.push_back(DURATA_MULTIPLE);
                    colors.push_back(COLORS);
                    break;
                case 3:
                    fileName.push_back(DURATA_ENEP);
                    colors.push_back(COLORS);
                    break;
                case 4:
                    fileName.push_back(PROBABILITA_MISURATA);
                    colors.push_back(COLORS);
                    break;
                case 5:
                    fileName.push_back(TEMPO_RITORNOX);
                    colors.push_back(COLORS);
                    break;
                case 6:
                    fileName.push_back(PROBABILITA_QUANTILEX_ENEP);
                    colors.push_back(COLORS);
                    break;
                case 7:
                    fileName.push_back(PROBABILITA_ECDFX);
                    colors.push_back(COLORS);
                    break;
                case 8:
                    fileName.push_back(PIOGGIAX);
                    colors.push_back(COLORS);
            }
        } else {
            checked.push_back(false);
            fileName.push_back("nulla");
            colors.push_back("nulla");
        }
    }
}
