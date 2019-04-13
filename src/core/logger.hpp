#ifndef LIBDMG_LOGGER_HPP
#define LIBDMG_LOGGER_HPP

#include <functional>
#include <string>

#ifdef DEBUG_LOG_INFO
#define LOG_MAX_STR_SIZE    1024

#define LOG_INFO(x)  LibDMG::Logger::instance()->logInfo(x)
#define LOG_ERROR(x) LibDMG::Logger::instance()->logError(x)
#define LOG_WARN(x) LibDMG::Logger::instance()->logWarn(x)
#define LOG_INFO_SET_CB(x)  LibDMG::Logger::instance()->setInfoCb(x);
#define LOG_ERROR_SET_CB(x) LibDMG::Logger::instance()->setErrorCb(x);
#define LOG_WARN_SET_CB(x) LibDMG::Logger::instance()->setWarnCb(x);
#else
#define LOG_MAX_STR_SIZE    0

#define LOG_INFO(x)
#define LOG_ERROR(x)
#define LOG_WARN(x)
#define LOG_INFO_SET_CB(x)
#define LOG_ERROR_SET_CB(x)
#define LOG_WARN_SET_CB(x)
#endif

namespace LibDMG
{
    class Logger
    {        
    public:
        static Logger* instance();
        void logInfo(const std::string& msg) { m_infoCb(msg); }
        void logError(const std::string& msg) { m_errorCb(msg); }
        void logWarn(const std::string& msg) { m_warnCb(msg); }
        void setInfoCb(const std::function<void(const std::string& msg)>& callback)
        {
            m_infoCb = callback;
        }

        void setErrorCb(const std::function<void(const std::string& msg)>& callback)
        {
            m_errorCb = callback;
        }

        void setWarnCb(const std::function<void(const std::string& msg)>& callback)
        {
            m_warnCb = callback;
        }
        
    private:
        static void defaultInfoCb(const std::string& msg);
        static void defaultErrorCb(const std::string& msg);
        static void defaultWarnCb(const std::string& msg);
    
        std::function<void(const std::string& msg)> m_infoCb;
        std::function<void(const std::string& msg)> m_errorCb;
        std::function<void(const std::string& msg)> m_warnCb;
    
        Logger();
    };
}

#endif // LIBDMG_LOGGER_HPP