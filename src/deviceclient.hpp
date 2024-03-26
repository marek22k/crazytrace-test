#ifndef DEVICECLIENT_HPP
#define DEVICECLIENT_HPP

#include <array>
#include <functional>
#include <string>
#include <boost/asio.hpp>

template<int BUFFER_SIZE> class DeviceClient
{
    public:
        explicit DeviceClient(
            boost::asio::any_io_executor ex,
            int native_handler,
            std::function<void(boost::system::error_code, std::string)>
                packet_handler,
            std::function<void(boost::system::error_code)> error_handler) :
            _device(ex, native_handler),
            _buffer(),
            _packet_handler(std::move(packet_handler)),
            _error_handler(std::move(error_handler))
        {
            this->read(); // flawfinder: ignore
        }

        void write(const std::string& data,
                   const std::function<void(boost::system::error_code,
                                            std::size_t bytes_transferred)>
                       write_handler,
                   const std::function<void(boost::system::error_code)>
                       write_error_handler)
        {
            this->_device.async_write_some(
                boost::asio::buffer(data, data.size()),
                [write_handler, write_error_handler](
                    boost::system::error_code ec, size_t bytes_transferred)
                {
                    if (ec)
                        write_error_handler(ec);
                    else
                        write_handler(ec, bytes_transferred);
                });
        }

    private:
        void read() // flawfinder: ignore
        {
            this->_device.async_read_some(
                boost::asio::buffer(this->_buffer),
                [this](boost::system::error_code ec, size_t bytes_transferred)
                {
                    if (ec)
                    {
                        this->_error_handler(ec);
                    }
                    else
                    {
                        std::string packet(this->_buffer.data(),
                                           bytes_transferred);
                        this->read(); // flawfinder: ignore

                        this->_packet_handler(ec, std::move(packet));
                    }
                });
        }

        boost::asio::posix::stream_descriptor _device;
        std::array<char, BUFFER_SIZE> _buffer;
        std::function<void(boost::system::error_code, std::string)>
            _packet_handler;
        std::function<void(boost::system::error_code)> _error_handler;
};

#endif
