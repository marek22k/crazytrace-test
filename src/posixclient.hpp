#ifndef POSIXCLIENT_HPP
#define POSIXCLIENT_HPP

#include <string>
#include <functional>
#include <utility>
#include <boost/asio.hpp>
#include <unistd.h>

template <int BUFFER_SIZE>
class PosixClient
{
    using Handler = std::function<void(boost::system::error_code, std::string)>;

    public:
        PosixClient(boost::asio::any_io_executor ex, int native_handler, Handler handler, Handler error_handler) :
            _device(ex, ::dup(native_handler)), _handler(std::move(handler)), _error_handler(std::move(error_handler))
        {
            this->start();
        }
    
    private:
        void start()
        {
            this->_device.async_read_some(
                boost::asio::buffer(this->_device),
                [this](boost::system::error_code ec, size_t bytes_transferred) {
                    if (ec) {
                        this->_error_handler(ec);
                    } else {
                        std::string packet(this->_device.data(), bytes_transferred);
                        start();

                        this->_handler(ec, std::move(packet));
                    };
            });
        }

        std::array<char, BUFFER_SIZE> _buffer;
        Handler _handler;
        Handler _error_handler;
        boost::asio::posix::stream_descriptor _device;
}

#endif
