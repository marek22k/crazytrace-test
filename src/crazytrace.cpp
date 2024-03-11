#include "crazytrace.hpp"

#include <iostream>
#include <tins/tins.h>

Crazytrace::Crazytrace(boost::asio::posix::stream_descriptor& device, NodeContainer nodecontainer) :
    _device(device), _nodecontainer(nodecontainer)
{
    
}

void Crazytrace::start()
{
    std::array<char, CRAZYTRACE_BUFFER_SIZE> buffer;

    this->_device.async_read_some(boost::asio::buffer(buffer),
                              [&](const boost::system::error_code& error, std::size_t bytes_transferred) {
                                  this->start();
                                  this->handle_packet(error, bytes_transferred, buffer);
                              });
}

void Crazytrace::handle_packet(const boost::system::error_code& error, std::size_t bytes_transferred, const std::array<char, CRAZYTRACE_BUFFER_SIZE>& buffer)
{
    if (error)
    {
        std::cerr << "Error in handle_packet: " << error.message() << std::endl;
        return;
    }
    // std::cout << "Received packet of size: " << bytes_transferred << std::endl;

    try
    {
        const uint8_t * raw_data = reinterpret_cast<const uint8_t *>(buffer.data());
        Tins::EthernetII packet(raw_data, bytes_transferred);

        NodeRequest request(packet);
        if (request.get_type() != NodeRequestType::UNKNOWN)
        {
            std::cout << request << std::endl;
            NodeReply reply = this->_nodecontainer.get_reply(packet);
            std::cout << reply << std::endl;
            if (reply.get_type() != NodeReplyType::NOREPLY)
            {
                std::string packet = reply.to_packet();
                this->_device.async_write_some(boost::asio::buffer(packet, packet.size()), [&](const boost::system::error_code& error, std::size_t bytes_transferred) {
                                    if (error)
                                    {
                                        std::cerr << "Failed to write packet" << std::endl;
                                    }
                                    else
                                    {
                                        std::cout << "Packet written, " << bytes_transferred << " bytes" << std::endl;
                                    }
                                });
            }
        }
    }
    catch (const Tins::malformed_packet&)
    {
        std::cerr << "Malformed packet" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
