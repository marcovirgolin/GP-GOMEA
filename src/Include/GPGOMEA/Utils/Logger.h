/*
 


 */

/* 
 * File:   Logger.h
 * Author: virgolin
 *
 * Created on July 3, 2018, 5:59 PM
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <string>
#include <fstream>

class Logger {
public:
   
    static Logger * GetInstance() {
        if (!instance)
            instance = new Logger();
        return instance;
    }
    
    void Log(std::string message, std::string file);
    
private:
    Logger() {};
    static Logger * instance;
    
    std::string ofile = "";
    std::ofstream ostream;
};

#endif /* LOGGER_H */

