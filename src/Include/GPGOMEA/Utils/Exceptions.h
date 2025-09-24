/*
 


 */

/* 
 * File:   Exceptions.h
 * Author: virgolin
 *
 * Created on June 27, 2018, 12:07 PM
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>

class NotImplementedException : public std::exception {
public:
    
    explicit NotImplementedException(const char* message):
      msg_(message)
      {
      }

    explicit NotImplementedException(const std::string& message):
      msg_(message)
      {}

    virtual ~NotImplementedException() throw (){}

    virtual const char* what() const throw (){
       return msg_.c_str();
    }

protected:
    std::string msg_;
};

#endif /* EXCEPTIONS_H */

