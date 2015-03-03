/* 
 * File:   SocketsException.h
 * Author: mike
 *
 * Created on March 2, 2015, 11:17 AM
 */

#ifndef SOCKETSEXCEPTION_H
#define	SOCKETSEXCEPTION_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <iostream>
#include <string>
#include <exception>

struct SocketsException : public std::exception {
    std::string exceptionMessage;

    SocketsException(std::string ss) : exceptionMessage(ss) {
    }

    ~SocketsException() throw () {
    }

    const char* what() const throw () {
        return exceptionMessage.c_str();
    }
};

#ifdef	__cplusplus
}
#endif

#endif	/* SOCKETSEXCEPTION_H */

