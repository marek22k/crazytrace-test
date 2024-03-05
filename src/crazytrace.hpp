#ifndef CRAZYTRACE_HPP
#define CRAZYTRACE_HPP

#define CRAZYTRACE_BUFFER_SIZE 1500

#include <array>
#include <cstdlib>
#include <boost/asio.hpp>

class Crazytrace
{
    public:
        Crazytrace(boost::asio::posix::stream_descriptor& device);
        void start();
    
    private:
        void handle_packet(const boost::system::error_code& error, std::size_t bytes_transferred, const std::array<char, CRAZYTRACE_BUFFER_SIZE>& buffer);

        boost::asio::posix::stream_descriptor& _device;
};

#endif
