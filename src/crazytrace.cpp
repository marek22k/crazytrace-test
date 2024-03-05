#include "crazytrace.hpp"

#include <iostream>
#include <tins/tins.h>

Crazytrace::Crazytrace(boost::asio::posix::stream_descriptor& device) : _device(device)
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
    std::cout << "Received packet of size: " << bytes_transferred << std::endl;

    try
    {
        const uint8_t * raw_data = reinterpret_cast<const uint8_t *>(buffer.data());
        Tins::EthernetII packet(raw_data, bytes_transferred);

        std::cout << "Packet Information:" << std::endl;
        std::cout << "Source MAC: " << packet.src_addr() << std::endl;
        std::cout << "Destination MAC: " << packet.dst_addr() << std::endl;

        if (packet.find_pdu<Tins::IP>())
        {
            // Zugriff auf das IP-PDU-Objekt, falls vorhanden
            const Tins::IP& ip = packet.rfind_pdu<Tins::IP>();

            // Ausgabe von Metainformationen zum IP-Paket
            std::cout << "IP Packet Information:" << std::endl;
            std::cout << "Source IP: " << ip.src_addr() << std::endl;
            std::cout << "Destination IP: " << ip.dst_addr() << std::endl;
            std::cout << "Protocol: " << ip.protocol() << std::endl;
        }
        else if (packet.find_pdu<Tins::IPv6>())
        {
            // Zugriff auf das IPv6-PDU-Objekt, falls vorhanden
            const Tins::IPv6& ipv6 = packet.rfind_pdu<Tins::IPv6>();

            // Ausgabe von Metainformationen zum IPv6-Paket
            std::cout << "IPv6 Packet Information:" << std::endl;
            std::cout << "Source IP: " << ipv6.src_addr() << std::endl;
            std::cout << "Destination IP: " << ipv6.dst_addr() << std::endl;
            std::cout << "Next Header: " << ipv6.next_header() << std::endl;
        }
        else
        {
            std::cerr << "No IP packet found in the EthernetII frame" << std::endl;
        }


        std::cout << std::endl;

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
