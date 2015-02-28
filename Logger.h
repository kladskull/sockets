
/* 
 * File:    Logger.h
 * Author:  Mike Curry
 * License: The MIT License (MIT)
 *
 * Created on February 27, 2015, 12:48 PM
 */

#ifndef LOGGER_H
#define	LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <time.h>

class Logger {
public:

    enum e_logType {
        LOG_FATAL, LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG, LOG_TRACE
    };

    // constructor

    explicit Logger(const char *fname = "app.log") {
        myFile.open(fname, std::ofstream::out | std::ofstream::app);
    }

    // destructor

    ~Logger() {
        if (myFile.is_open()) {
            myFile.close();
        }
    }

    // Overload << operator using log type
    friend Logger &operator<<(Logger &logger, const e_logType l_type) {
        
        logger.myFile << getTimeStamp() << " ";
        std::cout << getTimeStamp() << " ";
        
        switch (l_type) {
            case LOG_FATAL:
                logger.myFile << "FATAL ";
                std::cout << "FATAL ";
                break;

            case LOG_ERROR:
                logger.myFile << "ERROR ";
                std::cout << "ERROR ";
                break;

            case LOG_WARN:
                logger.myFile << "WARN  ";
                std::cout << "WARN  ";
                break;

            case LOG_INFO:
                logger.myFile << "INFO  ";
                std::cout << "INFO  ";
                break;

            case LOG_DEBUG:
                logger.myFile << "DEBUG ";
                std::cout << "DEBUG ";
                break;

            case LOG_TRACE:
                logger.myFile << "TRACE ";
                std::cout << "TRACE ";
                break;

            default:
                logger.myFile << "DEBUG ";
                std::cout << "DEBUG ";
                break;
        }

        return logger;
    }

    // Overload << operator using C style strings
    // No need for std::string objects here
    friend Logger &operator<<(Logger &logger, const char *text) {
        logger.myFile << text << std::endl;
        std::cout << text << std::endl;
        return logger;
    }

private:
    std::ofstream myFile;    

    // Get current date/time, format is YYYY-MM-DD HH:mm:ss
    static const std::string getTimeStamp() {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof (buf), "%Y-%m-%d %X", &tstruct);
        return buf;
    }

}; // class end

#endif	/* LOGGER_H */
