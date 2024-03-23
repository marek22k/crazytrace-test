#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>
#include <stdexcept>
#include <boost/log/trivial.hpp>
#include <tins/tins.h>
#include <yaml-cpp/yaml.h>
#include "loglevel.hpp"
#include "nodecontainer.hpp"
#include "postup_commands.hpp"

class Configuration
{
    public:
        explicit Configuration(const std::string& filename);
        [[nodiscard]] std::shared_ptr<NodeContainer>
            get_node_container() const noexcept;
        [[nodiscard]] LogLevel get_log_level() const noexcept;
        [[nodiscard]] const std::string& get_device_name() const noexcept;
        [[nodiscard]] const PostupCommands&
            get_postup_commands() const noexcept;

    private:
        void load(const std::string& filename);
        void load_log_level(const YAML::Node& node);
        void load_postup_commands(const YAML::Node& node);
        template<typename T,
                 typename std::enable_if_t<std::is_same_v<T, NodeInfo> ||
                                               std::is_same_v<T, NodeContainer>,
                                           int> = 0>
        void load_nodes(const YAML::Node& nodes_config,
                        std::shared_ptr<T> nodes,
                        bool mac = true);
        void validate_node_depth() const;

        std::string _device_name;
        std::shared_ptr<NodeContainer> _node_container;
        LogLevel _log_level;
        PostupCommands _postup_commands;
};

#endif
