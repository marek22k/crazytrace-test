#include "nodeinfo.hpp"

NodeInfo::NodeInfo()
{
    this->_hoplimit = 64;
}

std::vector<NodeInfo*> get_route_to(const Tins::IPv6Address& target_address)
{
    if (has_address(target_address))
    {
        std::vector<NodeInfo*> result;
        result.push_back(this);
        return result;
    }

    for (auto child_node = this->_nodes.begin(); child_node != this->_nodes.end(); child_node++)
    {
        std::vector<NodeInfo*> result = (*child_node)->get_route_to(target_address);
        if (! result.empty())
        {
            result.push_back(this);
            return result;
        }
    }

    return result;
}

int NodeInfo::get_hoplimit()
{
    return this->_hoplimit();
}

bool NodeInfo::has_address(Tins::IPv6Address& address)
{
    return this->_addresses.contains(address);
}

void NodeInfo::set_hoplimit(int hoplimit)
{
    this->_hoplimit = hoplimit;
}

void NodeInfo::add_node(NodeInfo node)
{
    this->_nodes.insert(node);
}

void NodeInfo::add_address(Tins::IPv6Address address)
{
    this->_addresses.insert(address);
}

