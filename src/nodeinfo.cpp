#include "nodeinfo.hpp"

NodeInfo::NodeInfo()
{
    this->_hoplimit = 64;
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

const Tins::IPv6Address& NodeInfo::get_address()
{
    return *this->_addresses.begin();
}

std::vector<std::shared_ptr<NodeInfo>> NodeInfo::get_route_to(const Tins::IPv6Address& destination_address)
{
    for (auto node = this->_nodes.begin(); node != this->_nodes.end(); node++)
    {
        if ( (*node)->has_address(destination_address) )
        {
            std::vector<std::shared_ptr<NodeInfo>> result;
            result.push_back(*node);
            return result;
        }
        else
        {
            std::vector<std::shared_ptr<NodeInfo>> result = (*node)->get_route_to(destination_address);
            result.push_back(*node);
            return result;
        }
    }

    return std::vector<std::shared_ptr<NodeInfo>>();
}
