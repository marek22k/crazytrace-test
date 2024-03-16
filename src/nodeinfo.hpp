#ifndef NODEINFO_HPP
#define NODEINFO_HPP

#include <iostream>
#include <ostream>
#include <memory>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <stdexcept>
#include <cstddef>
#include <tins/tins.h>
#include "randomgenerator.hpp"

class NodeInfo
{
    public:
        explicit NodeInfo();
        std::vector<std::shared_ptr<NodeInfo>> get_route_to(const Tins::IPv6Address& destination_address);
        int get_hoplimit() const noexcept;
        bool has_address(const Tins::IPv6Address& address);
        void set_hoplimit(int hoplimit);
        void set_mac_address(Tins::HWAddress<6> mac_address) noexcept;
        const Tins::HWAddress<6>& get_mac_address() const noexcept;
        void add_node(std::shared_ptr<NodeInfo> node);
        void add_address(Tins::IPv6Address address);
        const Tins::IPv6Address& get_address();
        std::size_t max_depth();
        void print(std::ostream& os, int layer = 0) const;

        friend std::ostream& operator<<(std::ostream& os, NodeInfo const & nodeinfo);

    private:
        std::vector<Tins::IPv6Address> _addresses;
        Tins::HWAddress<6> _mac_address;
        int _hoplimit;
        std::unordered_set<std::shared_ptr<NodeInfo>> _nodes;
        RandomGenerator _randomgenerator;
        bool _addressadded;
};

#endif
