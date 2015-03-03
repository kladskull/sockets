
#ifndef LOGGER_H
#define	LOGGER_H

#include <fstream>
#include <iostream>
#include <cstdarg>
#include <string>
#include <stdio.h>
#include <time.h>

#define LOGGER Logger::getLogger()

/**
 *   Singleton Logger Class.
 */
class Logger {
    
public:
    /**
     *   Logs a message
     *   @param sMessage message to be logged.
     */
    void Log(const std::string& sMessage);
    /**
     *   Variable Length Logger function
     *   @param format string for the message to be logged.
     */
    void Log(const char * format, ...);
    /**
     *   << overloaded function to Logs a message
     *   @param sMessage message to be logged.
     */
    Logger& operator<<(const std::string& sMessage);
    /**
     *   Function to create the instance of logger class.
     *   @return singleton object of Log class..
     */
    static Logger* getLogger();

private:
    /**
     *    Default constructor for the Logger class.
     */
    Logger();

    /**
     *   copy constructor for the Logger class.
     */
    Logger(const Logger&) {
    }; // copy constructor is private

    /**
     *   assignment operator for the Logger class.
     */
    Logger& operator=(const Logger&) {
        return *this;
    }; // assignment operator is private
    /**
     *   Log file name.
     **/
    static const std::string m_sFileName;
    /**
     *   Singleton logger class object pointer.
     **/
    static Logger* m_pThis;
    /**
     *   Log file stream object.
     **/
    static std::ofstream m_Logfile;
    
    const std::string getCurrentDateTime();
};

#endif	/* LOGGER_H */
