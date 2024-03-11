#include <iostream>
#include <memory>
#include <stdexcept>
#include <cstdlib>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

#include <unistd.h>

#include "tun_tap.hpp"
#include "crazytrace.hpp"
#include "nodecontainer.hpp"
#include "nodeinfo.hpp"

int main() {
    try {
        BOOST_LOG_TRIVIAL(info) << "libtuntap version: " << TUNTAP_VERSION_MAJOR << "." << TUNTAP_VERSION_MINOR << std::endl;
        int version = tuntap_version();
        int major = (version >> 8) & 0xFF;
        int minor = version & 0xFF;
        BOOST_LOG_TRIVIAL(info) << "libtuntap version: " << major << "." << minor << std::endl;

        std::unordered_set<std::shared_ptr<NodeInfo>> nodes;
        std::shared_ptr<NodeInfo> node1 = std::make_shared<NodeInfo>();
        node1->set_mac_address(Tins::HWAddress<6>("02:B1:4F:7E:9D:C3"));
        node1->add_address(Tins::IPv6Address("fe80::2c0a:baff:fe2c:3d54"));
        std::shared_ptr<NodeInfo> node1_node1 = std::make_shared<NodeInfo>();
        node1_node1->add_address(Tins::IPv6Address("fd00::3"));
        std::shared_ptr<NodeInfo> node1_node1_node1 = std::make_shared<NodeInfo>();
        node1_node1_node1->add_address(Tins::IPv6Address("fd00::4"));
        node1_node1->add_node(node1_node1_node1);
        node1->add_node(node1_node1);
        nodes.insert(node1);

        NodeContainer nodecontainer(nodes);

        boost::asio::io_context io;
        const ::size_t mtu = 1500;
        BOOST_LOG_TRIVIAL(debug) << "Create TUN device." << std::endl;
        tun_tap dev = tun_tap("crazytrace", tun_tap_mode::tap);
        BOOST_LOG_TRIVIAL(debug) << "Set MTU to " << mtu << "." << std::endl;
        dev.set_mtu(1500);
        BOOST_LOG_TRIVIAL(debug) << "Set the TUN device up." << std::endl;
        dev.up();

        boost::asio::posix::stream_descriptor tun_device(io, ::dup(dev.native_handler()));

        Crazytrace ct(tun_device, nodecontainer);
        ct.start();

        io.run();
    } catch (const std::exception &e) {
        BOOST_LOG_TRIVIAL(error) << "Error: " << e.what() << std::endl << "Exit program.";
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}
