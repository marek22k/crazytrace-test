#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include <unistd.h>

#include "tun_tap.hpp"
#include "crazytrace.hpp"
#include "nodecontainer.hpp"
#include "configuration.hpp"

int main(int argc, char *argv[]) {
    try {
        if (argc != 2)
            throw std::runtime_error("A configuration file must be specified.");
        
        std::string filename(argv[1]);
        Configuration config(filename);
        config.load();
        config.apply_log_level();

        BOOST_LOG_TRIVIAL(info) << "libtuntap version: " << TUNTAP_VERSION_MAJOR << "." << TUNTAP_VERSION_MINOR << std::endl;
        int version = tuntap_version();
        int major = (version >> 8) & 0xFF;
        int minor = version & 0xFF;
        BOOST_LOG_TRIVIAL(info) << "libtuntap version: " << major << "." << minor << std::endl;

        std::shared_ptr<NodeContainer> nodecontainer = config.get_node_container();

        std::ostringstream nodes_verbose;
        nodecontainer->print(nodes_verbose);
        BOOST_LOG_TRIVIAL(info) << nodes_verbose.str();

        boost::asio::io_context io;
        const ::size_t mtu = 1500;
        BOOST_LOG_TRIVIAL(debug) << "Create TUN device." << std::endl;
        tun_tap dev = tun_tap("crazytrace", tun_tap_mode::tap);
        BOOST_LOG_TRIVIAL(debug) << "Set MTU to " << mtu << "." << std::endl;
        dev.set_mtu(1500);
        BOOST_LOG_TRIVIAL(debug) << "Set the TUN device up." << std::endl;
        dev.up();

        Crazytrace ct(io.get_executor(), ::dup(dev.native_handler()), nodecontainer);

        io.run();
    } catch (const std::exception &e) {
        BOOST_LOG_TRIVIAL(fatal) << "Error: " << e.what() << std::endl << "Exit program.";
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
