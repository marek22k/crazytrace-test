#ifndef CRAZYTRACE_HPP
#define CRAZYTRACE_HPP

#include <iostream>
#include <array>
#include <memory>
#include <utility>
#include <string>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <tins/tins.h>
#include "noderequest.hpp"
#include "nodecontainer.hpp"

class Crazytrace
{
    public:
        explicit Crazytrace(boost::asio::any_io_executor ex, int native_handler, std::shared_ptr<NodeContainer> nodecontainer);
        void start();
    
    private:
        void _handle_error(const boost::system::error_code error);
        void _handle_packet(const boost::system::error_code error, const std::string& packet_data);

        boost::asio::posix::stream_descriptor _device;
        std::shared_ptr<NodeContainer> _nodecontainer;
        std::array<char, 1520> _buffer;
};

#endif
