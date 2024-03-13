#include "nodeinfo.hpp"


NodeInfo::NodeInfo() :
    _addresses(),
    _mac_address(),
    _hoplimit(64),
    _nodes(),
    _randomgenerator(0),
    _addressadded(false)
{}

int NodeInfo::get_hoplimit() const noexcept
{
    return this->_hoplimit;
}

void NodeInfo::set_mac_address(Tins::HWAddress<6> mac_address) noexcept
{
    this->_mac_address = mac_address;
}

const Tins::HWAddress<6>& NodeInfo::get_mac_address() const noexcept
{
    return this->_mac_address;
}

bool NodeInfo::has_address(const Tins::IPv6Address& address)
{
    return std::binary_search(this->_addresses.begin(), this->_addresses.end(), address);
}

void NodeInfo::set_hoplimit(int hoplimit)
{
    if (hoplimit < 0 || hoplimit > 255)
        throw std::invalid_argument("Hop limit outside the permitted value range");
    this->_hoplimit = hoplimit;
}

void NodeInfo::add_node(std::shared_ptr<NodeInfo> node)
{
    this->_nodes.insert(node);
}

void NodeInfo::add_address(Tins::IPv6Address address)
{
    this->_addressadded = true;
    this->_addresses.push_back(address);
}

const Tins::IPv6Address& NodeInfo::get_address()
{
    if (this->_addressadded)
    {
        /* Is called up each time the number of addresses is changed. However,
           it is not called after each address is added to allow multiple
           addresses to be added quickly. */
        ::size_t max_address = this->_addresses.size();
        if (max_address == 0)
            throw std::invalid_argument("Despite adding an address, none is available.");
        max_address--;

        this->_randomgenerator = RandomGenerator(max_address);
        std::sort(this->_addresses.begin(), this->_addresses.end());
        this->_addressadded = false;
    }
    ::size_t address_number = this->_randomgenerator.generate();
    return this->_addresses[address_number];
}

::size_t NodeInfo::max_depth()
{
    ::size_t max = 0;
    for (auto node = this->_nodes.begin(); node != this->_nodes.end(); node++)
    {
        ::size_t node_depth = (*node)->max_depth();
        if (node_depth > max)
            max = node_depth;
    }
    return max + 1;
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
            if (! result.empty())
            {
                result.push_back(*node);
                return result;
            }
        }
    }

    return std::vector<std::shared_ptr<NodeInfo>>();
}

void NodeInfo::print(std::ostream& os, int layer) const
{
    std::string tabs(layer, '\t');

    os << tabs << *this << std::endl;
    if (! this->_nodes.empty())
    {
        os << tabs << "Childs:" << std::endl;
        for (auto node = this->_nodes.begin(); node != this->_nodes.end(); node++)
        {
            (*node)->print(os, layer + 1);
        }
    }
}

std::ostream& operator<<(std::ostream& os, NodeInfo const & nodeinfo)
{
    os << "NodeInfo: hoplimit=" << nodeinfo._hoplimit;
    for (auto address = nodeinfo._addresses.begin(); address != nodeinfo._addresses.end(); address++)
    {
        os << " " << *address;
    }
    if (nodeinfo._mac_address != Tins::HWAddress<6>())
    {
        /* Is not empty address? */
        os << " " << nodeinfo._mac_address;
    }

    return os;
}
