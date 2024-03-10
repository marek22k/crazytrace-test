#include "nodeinfo.hpp"

NodeInfo::NodeInfo()
{
    this->_hoplimit = 64;
}

std::vector<std::shared_ptr<NodeInfo>> NodeInfo::get_route_to(const Tins::IPv6Address& target_address)
{
    for (auto child_node = this->_nodes.begin(); child_node != this->_nodes.end(); child_node++)
    {
        std::vector<std::shared_ptr<NodeInfo>> result = (*child_node)->get_route_to(target_address);
        if (! result.empty())
        {
            return result;
        }
    }

    // return std::move(result); ???
    return std::vector<std::shared_ptr<NodeInfo>>();
}

int NodeInfo::get_hoplimit()
{
    return this->_hoplimit;
}

void NodeInfo::set_mac_address(Tins::HWAddress<6> mac_address)
{
    this->_mac_address = mac_address;
}

const Tins::HWAddress<6>& NodeInfo::get_mac_address()
{
    return this->_mac_address;
}

bool NodeInfo::has_address(const Tins::IPv6Address& address)
{
    return this->_addresses.contains(address);
}

void NodeInfo::set_hoplimit(int hoplimit)
{
    // TODO: Check limits
    this->_hoplimit = hoplimit;
}

void NodeInfo::add_node(std::shared_ptr<NodeInfo> node)
{
    this->_nodes.insert(node);
}

void NodeInfo::add_address(Tins::IPv6Address address)
{
    this->_addresses.insert(address);
}

