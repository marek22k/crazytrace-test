#ifndef LOGLEVEL_HPP
#define LOGLEVEL_HPP

#include <string>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

class LogLevel
{
    public:
        explicit LogLevel(boost::log::trivial::severity_level log_level);
        explicit LogLevel(const std::string_view log_level_string);
        [[nodiscard]] const boost::log::trivial::severity_level&
            get_log_level() const noexcept;
        void apply() const;

    private:
        boost::log::trivial::severity_level _log_level;
};

#endif
