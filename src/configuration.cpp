#include "configuration.hpp"

Configuration::Configuration(const std::string& filename) :
    _node_container(std::make_shared<NodeContainer>())
{
    this->load(filename);
    this->validate_node_depth();
}

void Configuration::load(const std::string& filename)
{
    try
    {
        YAML::Node config = YAML::LoadFile(filename);
        this->load_log_level(config["log_level"]);

        YAML::Node device_name_node = config["device_name"];
        if (! device_name_node.IsDefined())
            throw std::runtime_error("device name is missing.");
        this->_device_name = device_name_node.as<std::string>();

        YAML::Node nodes_config = config["nodes"];
        this->load_nodes(nodes_config, this->_node_container);
    }
    catch (const YAML::Exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to load configuration file: " << e.what() << std::endl;
        throw std::runtime_error("Failed to load configuration file: YAML");
    }
}

void Configuration::validate_node_depth()
{
    ::size_t max_depth = this->_node_container->max_depth();
    if (max_depth > 255)
    {
        throw std::runtime_error("The nodes are too deep.");
    }
}

void Configuration::load_log_level(const YAML::Node& node)
{
    if (! node.IsDefined())
    {
        /* Default log level */
        this->_log_level = boost::log::trivial::info;
        return;
    }

    const std::string log_level_string = node.as<std::string>();

    if (log_level_string == "trace")
    {
        this->_log_level = boost::log::trivial::trace;
    }
    else if (log_level_string == "debug")
    {
        this->_log_level = boost::log::trivial::debug;
    }
    else if (log_level_string == "info")
    {
        this->_log_level = boost::log::trivial::info;
    }
    else if (log_level_string == "warning")
    {
        this->_log_level = boost::log::trivial::warning;
    }
    else if (log_level_string == "error")
    {
        this->_log_level = boost::log::trivial::error;
    }
    else if (log_level_string == "fatal")
    {
        this->_log_level = boost::log::trivial::fatal;
    }
    else
    {
        throw std::runtime_error("Failed to load configuration file: Unknown log level");
    }
}

template <typename T, typename std::enable_if<std::is_same<T, NodeInfo>::value || std::is_same<T, NodeContainer>::value, int>::type>
void Configuration::load_nodes(const YAML::Node& nodes_config, std::shared_ptr<T> nodes, bool mac)
{
    if (nodes_config.IsDefined() && !nodes_config.IsNull())
    {
        if (! nodes_config.IsSequence())
            throw std::runtime_error("Failed to load configuration file: Nodes is not a sequence.");

        for (auto node_config = nodes_config.begin(); node_config != nodes_config.end(); node_config++)
        {
            if ( (*node_config).IsNull() )
                continue;

            if (! (*node_config).IsMap())
                throw std::runtime_error("Failed to load configuration file: Node is not a map.");
            if (! (*node_config)["addresses"].IsDefined())
                throw std::runtime_error("Failed to load configuration file: Missing addresses attribute.");
            if (! (*node_config)["addresses"].IsSequence())
                throw std::runtime_error("Failed to load configuration file: Addresses is not a sequence.");
            if (mac)
            {
                if (! (*node_config)["mac"].IsDefined())
                    throw std::runtime_error("Failed to load configuration file: Missing mac attribute.");
            }

            std::shared_ptr<NodeInfo> node = std::make_shared<NodeInfo>();

            if (mac)
            {
                node->set_mac_address(Tins::HWAddress<6>((*node_config)["mac"].as<std::string>()));
            }

            YAML::Node addresses_config = (*node_config)["addresses"];
            for (auto address_config = addresses_config.begin(); address_config != addresses_config.end(); address_config++)
                node->add_address(Tins::IPv6Address((*address_config).as<std::string>()));

            YAML::Node hoplimit_config = (*node_config)["hoplimit"];
            if (hoplimit_config.IsDefined())
                node->set_hoplimit(hoplimit_config.as<int>());

            load_nodes((*node_config)["nodes"], node, false);
            nodes->add_node(node);
        }
    }
}

std::shared_ptr<NodeContainer> Configuration::get_node_container() const noexcept
{
    return this->_node_container;
}

boost::log::trivial::severity_level Configuration::get_log_level() const noexcept
{
    return this->_log_level;
}

const std::string& Configuration::get_device_name() const noexcept
{
    return this->_device_name;
}
