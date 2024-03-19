#include "loglevel.hpp"

LogLevel::LogLevel(boost::log::trivial::severity_level log_level) :
    _log_level(log_level)
{}

LogLevel::LogLevel(const std::string& log_level_string)
{
    if (log_level_string == "trace")
    {
        this->_log_level = boost::log::trivial::trace;
    }
    else if (log_level_string == "debug")
    {
        this->_log_level = boost::log::trivial::debug;
    }
    else if (log_level_string == "info")
    {
        this->_log_level = boost::log::trivial::info;
    }
    else if (log_level_string == "warning")
    {
        this->_log_level = boost::log::trivial::warning;
    }
    else if (log_level_string == "error")
    {
        this->_log_level = boost::log::trivial::error;
    }
    else if (log_level_string == "fatal")
    {
        this->_log_level = boost::log::trivial::fatal;
    }
    else
    {
        throw std::runtime_error("Unknown log level");
    }
}

void LogLevel::apply() const
{
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= this->_log_level);
}
