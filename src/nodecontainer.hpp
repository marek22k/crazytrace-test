#ifndef NODECONTAINER_HPP
#define NODECONTAINER_HPP

#include <vector>
#include <unordered_set>
#include <memory>
#include <iostream>
#include <ostream>
#include <boost/log/trivial.hpp>
#include <tins/tins.h>
#include "nodeinfo.hpp"
#include "noderequest.hpp"
#include "nodereply.hpp"

class NodeContainer
{
    public:
        NodeContainer();
        NodeContainer(std::unordered_set<std::shared_ptr<NodeInfo>> nodes);
        NodeReply get_reply(NodeRequest request);
        void set_nodes(std::unordered_set<std::shared_ptr<NodeInfo>> nodes);

        void print(std::ostream& os) const;
        friend std::ostream& operator<<(std::ostream& os, NodeContainer const & nodecontainer);

    private:
        std::vector<std::shared_ptr<NodeInfo>> get_route_to(const Tins::IPv6Address& destination_address);

        std::unordered_set<std::shared_ptr<NodeInfo>> _nodes;
};

#endif