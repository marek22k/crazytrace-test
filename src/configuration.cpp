#include "configuration.hpp"

Configuration::Configuration(std::string filename) :
    _filename(filename)
{}

void Configuration::load()
{
    try
    {
        YAML::Node config = YAML::LoadFile(this->_filename);
        this->load_log_level(config["log_level"]);

        std::unordered_set<std::shared_ptr<NodeInfo>> nodes;
        YAML::Node nodes_config = config["nodes"];
        this->load_nodes(nodes_config, true, nodes);
        this->_node_container = NodeContainer(nodes);
    }
    catch (const YAML::Exception& e)
    {
        BOOST_LOG_TRIVIAL(error) << "Failed to load configuration file: " << e.what() << std::endl;
        throw std::runtime_error("Failed to load configuration file: YAML");
    }
}

void Configuration::load_log_level(YAML::Node node)
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


void Configuration::load_nodes(YAML::Node nodes_config, bool mac, std::unordered_set<std::shared_ptr<NodeInfo>>& nodes)
{
    if (nodes_config.IsDefined())
    {
        if (! nodes_config.IsSequence())
            throw std::runtime_error("Failed to load configuration file: Nodes is not a sequence.");

        for (auto node_config = nodes_config.begin(); node_config != nodes_config.end(); node_config++)
        {
            //if (! (*node_config)["addresses"].IsDefined())
            //    throw std::runtime_error("Failed to load configuration file: Missing addresses attribute.");
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

            load_nodes(nodes_config["nodes"], false, nodes);
            nodes.insert(node);
        }
    }
}

const NodeContainer& Configuration::get_node_container()
{
    return this->_node_container;
}

boost::log::trivial::severity_level Configuration::get_log_level()
{
    return this->_log_level;
}

void Configuration::apply_log_level()
{
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= this->_log_level
    );
}
