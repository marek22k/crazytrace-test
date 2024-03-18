#include <memory>
#include <span>
#include <sstream>
#include <system_error>
#include <stdexcept>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <unistd.h>
#include "configuration.hpp"
#include "crazytrace.hpp"
#include "nodecontainer.hpp"
#include "tun_tap.hpp"

int main(int argc, char * argv[])
{
    try
    {
        const auto args = std::span(argv, static_cast<std::size_t>(argc));
        if (args.size() != 2)
            throw std::runtime_error("A configuration file must be specified.");

        const std::string filename(args[1]);
        const Configuration config(filename);

        boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                            config.get_log_level());

        BOOST_LOG_TRIVIAL(info) << "libtuntap version: " << TUNTAP_VERSION_MAJOR
                                << "." << TUNTAP_VERSION_MINOR;
        const int version = ::tuntap_version();
        const int major = (version >> 8) & 0xFF;
        const int minor = version & 0xFF;
        BOOST_LOG_TRIVIAL(info)
            << "libtuntap version: " << major << "." << minor;

#if defined(TINS_VERSION_MAJOR) && defined(TINS_VERSION_MINOR) && \
    defined(TINS_VERSION_PATCH)
        BOOST_LOG_TRIVIAL(info)
            << "libtins version: " << TINS_VERSION_MAJOR << "."
            << TINS_VERSION_MINOR << "." << TINS_VERSION_PATCH;
#endif

        const std::shared_ptr<NodeContainer> nodecontainer =
            config.get_node_container();

        std::ostringstream nodes_verbose;
        nodecontainer->print(nodes_verbose);
        BOOST_LOG_TRIVIAL(info) << nodes_verbose.str();

        boost::asio::io_context io;
        constexpr std::size_t mtu = 1500;
        BOOST_LOG_TRIVIAL(debug) << "Create TUN device.";
        tun_tap dev = tun_tap(config.get_device_name(), tun_tap_mode::tap);
        BOOST_LOG_TRIVIAL(debug) << "Set MTU to " << mtu << ".";
        dev.set_mtu(mtu);
        BOOST_LOG_TRIVIAL(debug) << "Set the TUN device up.";
        dev.up();

        const Crazytrace ct(
            io.get_executor(), ::dup(dev.native_handler()), nodecontainer);

        config.get_postup_commands().execute_commands();

        io.run();
    }
    catch (const std::exception& e)
    {
        BOOST_LOG_TRIVIAL(fatal) << "Error: " << e.what() << std::endl
                                 << "Exit program.";
        std::exit(EXIT_FAILURE); // NOLINT(concurrency-mt-unsafe)
    }
    return EXIT_SUCCESS;
}
