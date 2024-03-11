#include "nodecontainer.hpp"
#include <iostream>

NodeContainer::NodeContainer(std::unordered_set<std::shared_ptr<NodeInfo>> nodes) :
    _nodes(nodes)
{}

NodeReply NodeContainer::get_reply(NodeRequest request)
{

    switch(request.get_type())
    {
        case NodeRequestType::ICMP_ECHO_REQUEST:
            break;
        case NodeRequestType::ICMP_NDP:
        {
            /* Only the first level of nodes can have MAC addresses, as all
               other child nodes are "hidden" behind them and routing to them
               should be simulated. */
            for (auto node = this->_nodes.begin(); node != this->_nodes.end(); node++)
            {
                if ( (*node)->has_address(request.get_destination_address()) )
                {
                    /* We have found a node with the corresponding MAC address.
                       Other nodes with the same MAC address must not exist. */
                    NodeReply reply(
                        NodeReplyType::ICMP_NDP, (*node)->get_hoplimit(),
                        request.get_source_mac(), request.get_source_address(),
                        (*node)->get_mac_address(), request.get_destination_address()
                    );
                    return reply;
                }
            }
            return NodeReply(NodeReplyType::NOREPLY);
        }
        case NodeRequestType::UDP:
            break;
        default:
            return NodeReply(NodeReplyType::NOREPLY);
    }
}

void NodeContainer::set_nodes(std::unordered_set<std::shared_ptr<NodeInfo>> nodes)
{
    this->_nodes = nodes;
}
