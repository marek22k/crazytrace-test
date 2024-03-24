#ifndef NODEINFO_HPP
#define NODEINFO_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <stdexcept>
#include <cstddef>
#include <tins/tins.h>
#include "randomgenerator.hpp"

class NodeInfo
{
    public:
        explicit NodeInfo();
        [[nodiscard]] std::vector<std::shared_ptr<NodeInfo>>
            get_route_to(const Tins::IPv6Address& destination_address) const;
        void set_hoplimit(int hoplimit);
        void set_mac_address(Tins::HWAddress<6> mac_address) noexcept;
        void add_node(std::shared_ptr<NodeInfo> node);
        void add_address(Tins::IPv6Address address);
        [[nodiscard]] int get_hoplimit() const noexcept;
        [[nodiscard]] bool has_address(const Tins::IPv6Address& address);
        [[nodiscard]] const Tins::HWAddress<6>&
            get_mac_address() const noexcept;
        [[nodiscard]] const Tins::IPv6Address& get_address();
        [[nodiscard]] std::size_t max_depth() const;
        void print(std::ostream& os, int layer = 0) const;

        bool operator==(const NodeInfo& other) const;
        friend std::ostream& operator<<(std::ostream& os,
                                        NodeInfo const & nodeinfo);

    private:
        std::vector<Tins::IPv6Address> _addresses;
        Tins::HWAddress<6> _mac_address;
        int _hoplimit;
        std::vector<std::shared_ptr<NodeInfo>> _nodes;
        RandomGenerator _randomgenerator;
        bool _addressadded;
};

#endif
