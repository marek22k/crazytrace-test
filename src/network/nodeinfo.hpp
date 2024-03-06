#ifndef NODEINFO_HPP
#define NODEINFO_HPP

#include <tins/tins.h>

class NodeInfo
{
    public:
        NodeInfo();
        std::vector<NodeInfo*> get_route_to(const Tins::IPv6Address& target_address);
        int get_hoplimit();
        bool has_address(Tins::IPv6Address& address);
        void set_hoplimit(int hoplimit);
        void add_node(NodeInfo node);
        void add_address(Tins::IPv6Address address);

    private:
        std::unordered_set<Tins::IPv6Address> _addresses;
        int _hoplimit;
        std::unordered_set<NodeInfo> _nodes;
}

#endif
