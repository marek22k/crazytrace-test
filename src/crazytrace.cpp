#include "crazytrace.hpp"

Crazytrace::Crazytrace(boost::asio::any_io_executor ex,
                       int native_handler,
                       std::shared_ptr<NodeContainer> nodecontainer) :
    _nodecontainer(std::move(nodecontainer)),
    _client(
        std::move(ex),
        native_handler,
        [this](const boost::system::error_code& error,
               const std::string& packet)
        {
            this->_handle_packet(error, packet);
        },
        [this](const boost::system::error_code& error)
        {
            this->_handle_error(error);
        })
{
}

void Crazytrace::_handle_error(const boost::system::error_code error) const
{
    BOOST_LOG_TRIVIAL(fatal)
        << "Error in handle_packet: " << error.message() << std::endl;
}

void Crazytrace::_handle_packet(const boost::system::error_code,
                                const std::string& packet_data)
{
    BOOST_LOG_TRIVIAL(trace)
        << "Received packet of size: " << packet_data.size() << std::endl;

    try
    {
        const std::vector<uint8_t> raw_data(packet_data.begin(),
                                            packet_data.end());
        const Tins::EthernetII packet(raw_data.data(), raw_data.size());

        const NodeRequest request(packet);
        if (request.get_type() != NodeRequestType::UNKNOWN)
        {
            const NodeReply reply = this->_nodecontainer->get_reply(request);
            if (reply.get_type() != NodeReplyType::NOREPLY)
            {
                BOOST_LOG_TRIVIAL(debug) << request;
                BOOST_LOG_TRIVIAL(debug) << reply;
                const std::string reply_packet = reply.to_packet();
                this->_client.write(
                    reply_packet,
                    [](const boost::system::error_code&,
                       std::size_t bytes_transferred)
                    {
                        BOOST_LOG_TRIVIAL(trace)
                            << "Packet written, " << bytes_transferred
                            << " bytes";
                    },
                    [](const boost::system::error_code& error)
                    {
                        BOOST_LOG_TRIVIAL(warning)
                            << "Failed to write packet: " << error.message();
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
