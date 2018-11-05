//
// Created by giovanni on 27/09/18.
//

#ifndef MAPPEIRPI_ARGVMANAGER_H
#define MAPPEIRPI_ARGVMANAGER_H


#include <string>
#include <vector>

class ArgvManager {
public:
    ArgvManager(char *argv[]);
    bool isChecked(int pos);
    std::string getCumolative(int pos);
    std::string getFilename(int pos);
    std::string getColor(int pos);
private:
    void argDataParser(char**);

    int argN;
    int argSpeed;
    std::vector<std::string> argData;
    std::vector<std::string> fileName;
    std::vector<bool> checked;
    std::vector<std::string> cumolative;
    std::vector<std::string> colors;
};


#endif //MAPPEIRPI_ARGVMANAGER_H
