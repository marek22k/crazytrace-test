#include "configuration.hpp"

// NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
// _log_level is initialized in a function that is called directly from the
// constructor. If _log_level cannot be initialized, an error is thrown.
Configuration::Configuration(const std::string& filename) :
    _node_container(std::make_shared<NodeContainer>()),
    _log_level(boost::log::trivial::info)
{
    // NOLINTEND(cppcoreguidelines-pro-type-member-init)
    this->load(filename);
    this->validate_node_depth();
}

void Configuration::load(const std::string& filename)
{
    try
    {
        const YAML::Node config = YAML::LoadFile(filename);
        this->load_log_level(config["log_level"]);

        const YAML::Node device_name_node = config["device_name"];
        if (!device_name_node.IsDefined())
            throw std::runtime_error("device name is missing.");
        this->_device_name = device_name_node.as<std::string>();

        const YAML::Node post_up_command_node = config["post_up_commands"];
        this->load_postup_commands(post_up_command_node);

        const YAML::Node nodes_config = config["nodes"];
        this->load_nodes(nodes_config, this->_node_container);
    }
    catch (const YAML::Exception& e)
    {
        BOOST_LOG_TRIVIAL(error)
            << "Failed to load configuration file: " << e.what() << std::endl;
        throw std::runtime_error("Failed to load configuration file: YAML");
    }
}

void Configuration::validate_node_depth() const
{
    const std::size_t max_depth = this->_node_container->max_depth();
    if (max_depth > 255)
    {
        throw std::runtime_error("The nodes are too deep.");
    }
}

void Configuration::load_log_level(const YAML::Node& node)
{
    if (!node.IsDefined())
        return;

    const std::string log_level_string = node.as<std::string>();
    this->_log_level = LogLevel(log_level_string);
}

void Configuration::load_postup_commands(const YAML::Node& node)
{
    if (node.IsDefined())
    {
        if (!node.IsSequence())
            throw std::runtime_error("post up commands must be an array.");

        for (const auto& command_node : node)
        {
            this->_postup_commands.add_postup_command(
                command_node.as<std::string>());
        }
    }
}

template<typename T,
         typename std::enable_if_t<std::is_same_v<T, NodeInfo> ||
                                       std::is_same_v<T, NodeContainer>,
                                   int>>
void Configuration::load_nodes(const YAML::Node& nodes_config,
                               std::shared_ptr<T> nodes,
                               bool mac)
{
    if (nodes_config.IsDefined() && !nodes_config.IsNull())
    {
        if (!nodes_config.IsSequence())
            throw std::runtime_error(
                "Failed to load configuration file: Nodes is not a sequence.");

        for (const auto& node_config : nodes_config)
        {
            if (node_config.IsNull())
                continue;

            if (!node_config.IsMap())
                throw std::runtime_error(
                    "Failed to load configuration file: Node is not a map.");
            if (!node_config["addresses"].IsDefined())
                throw std::runtime_error("Failed to load configuration file: "
                                         "Missing addresses attribute.");
            if (!node_config["addresses"].IsSequence())
                throw std::runtime_error("Failed to load configuration file: "
                                         "Addresses is not a sequence.");
            if (mac && !node_config["mac"].IsDefined())
                throw std::runtime_error("Failed to load configuration "
                                         "file: Missing mac attribute.");

            const auto node = std::make_shared<NodeInfo>();

            if (mac)
            {
                node->set_mac_address(
                    Tins::HWAddress<6>(node_config["mac"].as<std::string>()));
            }

            for (const YAML::Node addresses_config = node_config["addresses"];
                 const auto& address_config : addresses_config)
                node->add_address(
                    Tins::IPv6Address(address_config.as<std::string>()));

            if (const YAML::Node hoplimit_config = node_config["hoplimit"];
                hoplimit_config.IsDefined())
                node->set_hoplimit(hoplimit_config.as<int>());

            load_nodes(node_config["nodes"], node, false);
            nodes->add_node(node);
        }
    }
}

std::shared_ptr<NodeContainer>
    Configuration::get_node_container() const noexcept
{
    return this->_node_container;
}

LogLevel Configuration::get_log_level() const noexcept
{
    return this->_log_level;
}

const std::string& Configuration::get_device_name() const noexcept
{
    return this->_device_name;
}

const PostupCommands& Configuration::get_postup_commands() const noexcept
{
    return this->_postup_commands;
}
