#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <array>

#include <boost/asio.hpp>

#include "tun_tap.hpp"

const std::size_t BUFFER_SIZE = 65535;

void handle_packet(const boost::system::error_code& error, std::size_t bytes_transferred, const std::array<char, BUFFER_SIZE>& buffer);
void start_async_read(boost::asio::posix::stream_descriptor& async_dev);

void handle_packet(const boost::system::error_code& error, std::size_t bytes_transferred, const std::array<char, BUFFER_SIZE>& buffer) {
    if (!error) {
        std::cout << "Received packet of size: " << bytes_transferred << std::endl;
        std::cout << "Data: " << std::string(buffer.data(), bytes_transferred) << std::endl;
    } else {
        std::cerr << "Error in handle_packet: " << error.message() << std::endl;
    }
}

void start_async_read(boost::asio::posix::stream_descriptor& async_dev) {
    std::array<char, BUFFER_SIZE> buffer;

    async_dev.async_read_some(boost::asio::buffer(buffer),
                              [&](const boost::system::error_code& error, std::size_t bytes_transferred) {
                                  handle_packet(error, bytes_transferred, buffer);
                                  start_async_read(async_dev);
                              });
}

int main() {
    try {
        std::clog << "libtuntap version: " << TUNTAP_VERSION_MAJOR << "." << TUNTAP_VERSION_MINOR << std::endl;
        int version = tuntap_version();
        int major = (version >> 8) & 0xFF;
        int minor = version & 0xFF;
        std::clog << "libtuntap version: " << major << "." << minor << std::endl;

        boost::asio::io_context io;
        tun_tap dev = tun_tap("crazytrace", tun_tap_mode::tun);
        dev.up();

        boost::asio::posix::stream_descriptor async_dev(io, dup(dev.native_handler()));

        start_async_read(async_dev);

        io.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl << "Exit program.";
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
