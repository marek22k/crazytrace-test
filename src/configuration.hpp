#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <stdexcept>
#include <memory>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <tins/tins.h>
#include <yaml-cpp/yaml.h>
#include "nodecontainer.hpp"

class Configuration
{
    public:
        Configuration(std::string filename);
        void load();
        const NodeContainer& get_node_container();
        boost::log::trivial::severity_level get_log_level();
        void apply_log_level();
    
    private:
        void load_log_level(YAML::Node node);
        void load_nodes(YAML::Node nodes_config, bool mac, std::unordered_set<std::shared_ptr<NodeInfo>>& nodes);

        std::string _filename;
        NodeContainer _node_container;
        boost::log::trivial::severity_level _log_level;
};

#endif
