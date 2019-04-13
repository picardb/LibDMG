#include "logger.hpp"
#include <iostream>

using namespace std;
using namespace LibDMG;

Logger* Logger::instance() {
    static Logger instance;
    
    return &instance;
}

Logger::Logger() : 
    m_errorCb(defaultErrorCb),
    m_infoCb(defaultInfoCb),
    m_warnCb(defaultWarnCb)
{

}

void Logger::defaultInfoCb(const string& msg)
{
    cout << "[INFO ]  " << msg << endl;
}

void Logger::defaultErrorCb(const string& msg)
{
    cout << "[ERROR] " << msg << endl;
}

void Logger::defaultWarnCb(const string& msg)
{
    cout << "[WARN ] " << msg << endl;
}