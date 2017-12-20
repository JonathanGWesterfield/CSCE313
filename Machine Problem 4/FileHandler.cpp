//
// Created by Jonathan G. Westerfield on 11/2/17.
//

#include "FileHandler.h"

using namespace std;

FileHandler::FileHandler(std::vector<int> data, std::string personName, int personID)
{
    this->data = data;
    this->name = personName;
    this->personID = personID;
}

void FileHandler::printToXLS()
{
    if(personID == 0) // Joe's ID
    {
        ofstream joeFile;
        joeFile.open("JoeFile.xls");
        /* Format for exporting to an excel file: \t means column and \r means
         * move to the next row */

        joeFile << "Count" << "\t" << "HistValue" << "\r";
        for(int i = 0; i < data.size(); i++)
        {
            joeFile << i << "\t" << data[i] << "\r";
        }
        joeFile.close();
    }
    else if(personID == 1) // Joe's ID
    {
        ofstream janeFile;
        janeFile.open("JaneFile.xls");
        /* Format for exporting to an excel file: \t means column and \r means
         * move to the next row */

        janeFile << "Count" << "\t" << "HistValue" << "\r";
        for(int i = 0; i < data.size(); i++)
        {
            janeFile << i << "\t" << data[i] << "\r";
        }
        janeFile.close();
    }
    else if(personID == 2) // Joe's ID
    {
        ofstream johnFile;
        johnFile.open("JohnFile.xls");
        /* Format for exporting to an excel file: \t means column and \r means
         * move to the next row */

        johnFile << "Count" << "\t" << "HistValue" << "\r";
        for(int i = 0; i < data.size(); i++)
        {
            johnFile << i << "\t" << data[i] << "\r";
        }
        johnFile.close();
    }
}