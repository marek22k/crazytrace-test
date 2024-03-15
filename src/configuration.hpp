#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <stdexcept>
#include <memory>
#include <type_traits>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <tins/tins.h>
#include <yaml-cpp/yaml.h>
#include "nodecontainer.hpp"

class Configuration
{
    public:
        explicit Configuration(const std::string& filename);
        void load();
        void validate_node_depth();
        std::shared_ptr<NodeContainer> get_node_container() const noexcept;
        boost::log::trivial::severity_level get_log_level() const noexcept;
        const std::string& get_device_name() const noexcept;
        void apply_log_level();
    
    private:
        void load_log_level(const YAML::Node& node);
        template <typename T, typename std::enable_if<std::is_same<T, NodeInfo>::value || std::is_same<T, NodeContainer>::value, int>::type = 0>
        void load_nodes(const YAML::Node& nodes_config, std::shared_ptr<T> nodes, bool mac = true);

        const std::string& _filename;
        std::string _device_name;
        std::shared_ptr<NodeContainer> _node_container;
        boost::log::trivial::severity_level _log_level;
};

#endif
