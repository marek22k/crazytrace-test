#include "crazytrace.hpp"

Crazytrace::Crazytrace(boost::asio::any_io_executor ex, int native_handler, std::shared_ptr<NodeContainer> nodecontainer) :
     _device(ex, native_handler), _nodecontainer(std::move(nodecontainer)), _buffer()
{
    this->start();
}

void Crazytrace::start()
{
    this->_device.async_read_some(
        boost::asio::buffer(this->_buffer),
        [this](boost::system::error_code ec, size_t bytes_transferred) {
            if (ec) {
                this->_handle_error(ec);
            } else {
                std::string packet(this->_buffer.data(), bytes_transferred); // NOLINT
                start();

                this->_handle_packet(ec, std::move(packet)); // NOLINT
            };
    });
}

void Crazytrace::_handle_error(const boost::system::error_code error)
{
    BOOST_LOG_TRIVIAL(error) << "Error in handle_packet: " << error.message() << std::endl;
}

void Crazytrace::_handle_packet(const boost::system::error_code, const std::string& packet_data)
{
    BOOST_LOG_TRIVIAL(trace) << "Received packet of size: " << packet_data.size() << std::endl;

    try
    {
        const std::vector<uint8_t> raw_data(packet_data.begin(), packet_data.end());
        const Tins::EthernetII packet(raw_data.data(), raw_data.size());

        const NodeRequest request(packet);
        if (request.get_type() != NodeRequestType::UNKNOWN)
        {
            const NodeReply reply = this->_nodecontainer->get_reply(request);
            if (reply.get_type() != NodeReplyType::NOREPLY)
            {
                BOOST_LOG_TRIVIAL(debug) << request;
                BOOST_LOG_TRIVIAL(debug) << reply;
                std::string reply_packet = reply.to_packet();
                this->_device.async_write_some(boost::asio::buffer(reply_packet, reply_packet.size()), [&](const boost::system::error_code& error, std::size_t bytes_transferred) {
                                    if (error)
                                    {
                                        BOOST_LOG_TRIVIAL(warning) << "Failed to write packet: " << error.message() << std::endl;
                                    }
                                    else
                                    {
                                        BOOST_LOG_TRIVIAL(trace) << "Packet written, " << bytes_transferred << " bytes" << std::endl;
                                    }
                                });
            }
        }
    }
    catch (const Tins::malformed_packet&)
    {
        BOOST_LOG_TRIVIAL(warning) << "Malformed packet" << std::endl;
    }
    catch (const std::exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Error: " << e.what() << std::endl;
    }
}
