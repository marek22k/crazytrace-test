#ifndef NODECONTAINER_HPP
#define NODECONTAINER_HPP

#include <memory>
#include "nodeinfo.hpp"

class NodeContainer
{
    public:
        std::shared_ptr<NodeInfo> root;
};

#endif