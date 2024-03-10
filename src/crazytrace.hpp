#ifndef CRAZYTRACE_HPP
#define CRAZYTRACE_HPP

#define CRAZYTRACE_BUFFER_SIZE 1500

#include <array>
#include <memory>
#include <cstdlib>
#include <boost/asio.hpp>
#include "noderequest.hpp"
#include "nodecontainer.hpp"

class Crazytrace
{
    public:
        Crazytrace(boost::asio::posix::stream_descriptor& device, std::shared_ptr<NodeContainer> nodecontainer);
        void start();
    
    private:
        void handle_packet(const boost::system::error_code& error, std::size_t bytes_transferred, const std::array<char, CRAZYTRACE_BUFFER_SIZE>& buffer);

        boost::asio::posix::stream_descriptor& _device;
        std::shared_ptr<NodeContainer> _nodecontainer;
};

#endif
