/*
 


 */

/* 
 * File:   Logger.cpp
 * Author: virgolin
 * 
 * Created on July 3, 2018, 5:59 PM
 */

#include "Logger.h"

using namespace std;

Logger * Logger::instance = NULL;

void Logger::Log(std::string message, std::string file) {

    if (file.compare(ofile) != 0) {
        if (ostream.is_open())
            ostream.close();
        ofile = file;
        ostream.open(ofile, ios_base::app | ios_base::ate);
    }
    ostream << message << endl;
    
}
