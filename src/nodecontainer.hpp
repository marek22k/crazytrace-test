#ifndef NODECONTAINER_HPP
#define NODECONTAINER_HPP

#include <algorithm>
#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <boost/log/trivial.hpp>
#include <tins/tins.h>
#include "nodeinfo.hpp"
#include "nodereply.hpp"
#include "noderequest.hpp"

class NodeContainer
{
    public:
        NodeReply get_reply(const NodeRequest& request);
        void add_node(std::shared_ptr<NodeInfo> node) noexcept;
        [[nodiscard]] std::size_t max_depth() const;

        void print(std::ostream& os) const;
        bool operator==(const NodeContainer& other) const;
        friend std::ostream& operator<<(std::ostream& os,
                                        const NodeContainer& nodecontainer);

    private:
        std::vector<std::shared_ptr<NodeInfo>>
            get_route_to(const Tins::IPv6Address& destination_address) const;
        std::vector<std::shared_ptr<NodeInfo>> _nodes;
};

#endif
