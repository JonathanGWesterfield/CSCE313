//
// Created by Jonathan G. Westerfield on 11/2/17.
//

#ifndef TRY2_FILEHANDLER_H
#define TRY2_FILEHANDLER_H

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

class FileHandler
{
private:
    std::vector<int> data;
    std::string name;
    int personID;

    // std::ofstream joeFile; // ("JoeFile.xls", std::ofstream::out);
    // std::ofstream janeFile; // ("JaneFile.xls");
    // std::ofstream johnFile; // ("JohnFile.xls");

public:
    FileHandler(std::vector<int> data, std::string personName, int personID);
    // ~FileHandler();
    void printToXLS();
};

//TODO: Finish up the excel stuff and then write the report.

#endif //TRY2_FILEHANDLER_H
