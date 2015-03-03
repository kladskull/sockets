/* 
 * File:   Logger.cpp
 * Author: Mike Curry
 * 
 * Created on February 27, 2015, 8:56 PM
 */

#include "Logger.h"

using namespace std;

//#include"Utilities.h"

const string Logger::m_sFileName = "sockets.log";
Logger* Logger::m_pThis = NULL;
ofstream Logger::m_Logfile;

Logger::Logger() {
}

Logger* Logger::getLogger() {
    if (m_pThis == NULL) {
        m_pThis = new Logger();
        m_Logfile.open(m_sFileName.c_str(), ios::out | ios::app);
    }
    return m_pThis;
}

void Logger::Log(const char * format, ...) {
    char sMessage[256];
    va_list args;
    va_start(args, format);
    vsprintf(sMessage, format, args);
    m_Logfile << endl << getCurrentDateTime() << ":\t";
    m_Logfile << sMessage;
    va_end(args);
}

void Logger::Log(const string& sMessage) {
    m_Logfile << endl << getCurrentDateTime() << ":\t";
    m_Logfile << sMessage;
}

Logger& Logger::operator<<(const string& sMessage) {
    m_Logfile << endl << getCurrentDateTime() << ":\t";
    m_Logfile << sMessage;
    return *this;
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss

const string Logger::getCurrentDateTime() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof (buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}