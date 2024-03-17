#include "nodecontainer.hpp"

NodeReply NodeContainer::get_reply(const NodeRequest& request)
{
    switch (request.get_type())
    {
        case NodeRequestType::ICMP_ECHO_REQUEST:
        case NodeRequestType::UDP:
        {
            std::vector<std::shared_ptr<NodeInfo>> route =
                this->get_route_to(request.get_destination_address());
            if (route.empty())
            {
                return NodeReply(NodeReplyType::NOREPLY);
            }

            const Tins::HWAddress<6> source_mac =
                route.back()->get_mac_address();

            const int hoplimit = request.get_hoplimit();
            if (static_cast<std::size_t>(hoplimit) >= route.size())
            {
                /* target reached */
                const std::shared_ptr<NodeInfo> reached_node = route[0];

                // Both variables undergo a value check during initialization so
                // that neither is greater than 255. It is therefore safe to
                // convert them into an int.
                const int reply_hoplimit = reached_node->get_hoplimit() -
                                           static_cast<int>(route.size()) + 1;
                if (reply_hoplimit <= 0)
                    return NodeReply(NodeReplyType::NOREPLY);

                switch (request.get_type())
                {
                    case NodeRequestType::ICMP_ECHO_REQUEST:
                    {
                        NodeReply reply(NodeReplyType::ICMP_ECHO_REPLY,
                                        request.get_source_mac(),
                                        request.get_source_address(),
                                        source_mac,
                                        reached_node->get_address());
                        reply.icmp_echo_reply(request.get_icmp_identifier(),
                                              request.get_icmp_sequence(),
                                              request.get_payload());
                        reply.set_hoplimit(reply_hoplimit);
                        return reply;
                    }
                    case NodeRequestType::UDP:
                    {
                        NodeReply reply(NodeReplyType::ICMP_PORT_UNREACHABLE,
                                        request.get_source_mac(),
                                        request.get_source_address(),
                                        source_mac,
                                        reached_node->get_address());
                        reply.udp_response(request.get_payload(),
                                           request.get_udp_dport(),
                                           request.get_udp_sport());
                        reply.packet_reassembly(
                            request.get_destination_address());
                        reply.set_hoplimit(reply_hoplimit);
                        return reply;
                    }
                    default:
                        [[unlikely]] throw std::runtime_error(
                            "An impossible error has occurred. A reply was "
                            "requested for ICMP_ECHO_REQUEST or UDP, but the "
                            "request has suddenly changed.");
                        break;
                }
            }
            else
            {
                /* hoplimit exceeded */
                const int reached_node_number =
                    static_cast<int>(route.size()) - request.get_hoplimit();
                const std::shared_ptr<NodeInfo> reached_node =
                    route[reached_node_number];

                const int reply_hoplimit =
                    reached_node->get_hoplimit() - hoplimit + 1;
                if (reply_hoplimit <= 0)
                    return NodeReply(NodeReplyType::NOREPLY);

                switch (request.get_type())
                {
                    case NodeRequestType::ICMP_ECHO_REQUEST:
                    {
                        NodeReply reply(
                            NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST,
                            request.get_source_mac(),
                            request.get_source_address(),
                            source_mac,
                            reached_node->get_address());
                        reply.icmp_echo_reply(request.get_icmp_identifier(),
                                              request.get_icmp_sequence(),
                                              request.get_payload());
                        reply.packet_reassembly(
                            request.get_destination_address());
                        reply.set_hoplimit(reply_hoplimit);
                        return reply;
                    }
                    case NodeRequestType::UDP:
                    {
                        NodeReply reply(NodeReplyType::ICMP_TIME_EXCEEDED_UDP,
                                        request.get_source_mac(),
                                        request.get_source_address(),
                                        source_mac,
                                        reached_node->get_address());
                        reply.udp_response(request.get_payload(),
                                           request.get_udp_dport(),
                                           request.get_udp_sport());
                        reply.packet_reassembly(
                            request.get_destination_address());
                        reply.set_hoplimit(reply_hoplimit);
                        return reply;
                    }
                    default:
                        [[unlikely]] throw std::runtime_error(
                            "An impossible error has occurred. A reply was "
                            "requested for ICMP_ECHO_REQUEST or UDP, but the "
                            "request has suddenly changed.");
                        break;
                }
            }
            break;
        }
        case NodeRequestType::ICMP_NDP:
        {
            /* Only the first level of nodes can have MAC addresses, as all
               other child nodes are "hidden" behind them and routing to them
               should be simulated. */
            auto found_node = std::find_if(
                this->_nodes.begin(),
                this->_nodes.end(),
                [&](const std::shared_ptr<NodeInfo>& node)
                {
                    return node->has_address(request.get_destination_address());
                });
            if (found_node != this->_nodes.end())
            {
                /* We have found a node with the corresponding MAC address.
                    Other nodes with the same MAC address must not exist. */
                NodeReply reply(NodeReplyType::ICMP_NDP,
                                request.get_source_mac(),
                                request.get_source_address(),
                                (*found_node)->get_mac_address(),
                                request.get_destination_address());
                return reply;
            }
            return NodeReply(NodeReplyType::NOREPLY);
        }
        default:
            return NodeReply(NodeReplyType::NOREPLY);
    }

    return NodeReply(NodeReplyType::NOREPLY);
}

void NodeContainer::add_node(std::shared_ptr<NodeInfo> node) noexcept
{
    this->_nodes.insert(node);
}

std::size_t NodeContainer::max_depth()
{
    std::size_t max = 0;
    for (const auto& node : this->_nodes)
    {
        const std::size_t node_depth = node->max_depth();
        max = std::max(max, node_depth);
    }
    return max;
}

std::vector<std::shared_ptr<NodeInfo>>
    NodeContainer::get_route_to(const Tins::IPv6Address& destination_address)
{
    for (const auto& node : this->_nodes)
    {
        if (node->has_address(destination_address))
        {
            std::vector<std::shared_ptr<NodeInfo>> result;
            result.push_back(node);
            return result;
        }

        std::vector<std::shared_ptr<NodeInfo>> result =
            node->get_route_to(destination_address);
        if (!result.empty())
        {
            result.push_back(node);
            return result;
        }
    }

    return {};
}

void NodeContainer::print(std::ostream& os) const
{
    os << *this << std::endl;
    if (!this->_nodes.empty())
    {
        os << "Childs:" << std::endl;
        for (const auto& node : this->_nodes)
        {
            node->print(os, 1);
        }
    }
}

std::ostream& operator<<(std::ostream& os, const NodeContainer& nodecontainer)
{
    os << "NodeContainer: " << nodecontainer._nodes.size() << " childnodes";
    return os;
}
