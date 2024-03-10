#ifndef NODECONTAINER_HPP
#define NODECONTAINER_HPP

#include <memory>
#include <tins/tins.h>
#include "nodeinfo.hpp"
#include "nodereply.hpp"

class NodeContainer
{
    public:
        NodeReply get_reply(NodeContainer root);


    private:
        std::shared_ptr<NodeInfo> _root;
};

#endif