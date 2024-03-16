#ifndef POSTUP_COMMANDS_HPP
#define POSTUP_COMMANDS_HPP

#include <boost/log/trivial.hpp>
#include <boost/process.hpp>

class PostupCommands
{
    public:
        void add_postup_command(std::string command);
        void execute_commands() const;

    private:
        std::vector<std::string> _postup_commands;
};

#endif
