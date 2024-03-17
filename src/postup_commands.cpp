#include "postup_commands.hpp"

void PostupCommands::add_postup_command(std::string command)
{
    this->_postup_commands.push_back(command);
}

void PostupCommands::execute_commands() const
{
    for (const auto& postup_command : this->_postup_commands)
    {
        BOOST_LOG_TRIVIAL(debug) << "Execute post up command: " << postup_command << std::endl;
        std::error_code ec;
        boost::process::child child(
            postup_command,
            boost::process::std_out > boost::process::null,
            boost::process::std_err > boost::process::null,
            ec
        );
        child.wait();
        BOOST_LOG_TRIVIAL(debug) << "Post up command result: " << child.exit_code() << std::endl;
        if (child.exit_code() != 0 || ec)
        {
            if (ec)
            {
                BOOST_LOG_TRIVIAL(fatal) << "Failed to execute post up command: " << ec.message() << std::endl;
            }
            throw std::runtime_error("Failed to execute post up command.");
        }
    }
}
