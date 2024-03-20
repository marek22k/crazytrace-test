#include <gtest/gtest.h>
#include <sstream>
#include <tins/tins.h>
#include "nodeinfo.hpp"

TEST(NodeInfoTest, MacAddress)

{
    const Tins::HWAddress<6> mac_address(std::string("52:54:00:b2:fa:7f"));
    NodeInfo nodeinfo;
    nodeinfo.set_mac_address(mac_address);
    EXPECT_EQ(mac_address, nodeinfo.get_mac_address());
    EXPECT_EQ(mac_address.to_string(), nodeinfo.get_mac_address().to_string());
}

TEST(NodeInfoTest, HopLimit)
{
    constexpr int hoplimit = 150;
    NodeInfo nodeinfo;
    EXPECT_EQ(64, nodeinfo.get_hoplimit());

    nodeinfo.set_hoplimit(hoplimit);
    EXPECT_EQ(hoplimit, nodeinfo.get_hoplimit());
}

TEST(NodeInfoTest, IPv6Address)
{
    const Tins::IPv6Address address1(std::string("fd00::1"));
    const Tins::IPv6Address address2(std::string("fd00::2"));
    NodeInfo nodeinfo;

    nodeinfo.add_address(address1);
    EXPECT_EQ(address1, nodeinfo.get_address());
    EXPECT_EQ(address1.to_string(), nodeinfo.get_address().to_string());

    nodeinfo.add_address(address2);
    Tins::IPv6Address random_address = nodeinfo.get_address();

    while (random_address != address1)
        random_address = nodeinfo.get_address();
    EXPECT_EQ(random_address, address1);

    while (random_address != address2)
        random_address = nodeinfo.get_address();
    EXPECT_EQ(random_address, address2);
}

TEST(NodeInfoTest, HasAddress)
{
    const Tins::IPv6Address address1(std::string("fd00::1"));
    const Tins::IPv6Address address2(std::string("fd00::2"));
    NodeInfo nodeinfo;

    EXPECT_FALSE(nodeinfo.has_address(address1));
    EXPECT_FALSE(nodeinfo.has_address(address2));

    nodeinfo.add_address(address1);
    EXPECT_TRUE(nodeinfo.has_address(address1));
    EXPECT_FALSE(nodeinfo.has_address(address2));

    nodeinfo.add_address(address2);
    EXPECT_TRUE(nodeinfo.has_address(address1));
    EXPECT_TRUE(nodeinfo.has_address(address2));
}

TEST(NodeInfoTest, Output)
{
    const Tins::IPv6Address address1(std::string("fd00::1"));
    const Tins::IPv6Address address2(std::string("fd00::2"));
    NodeInfo nodeinfo;
    nodeinfo.add_address(address1);

    std::ostringstream test_output;

    test_output << nodeinfo;
    EXPECT_EQ(test_output.str(), "NodeInfo: hoplimit=64 fd00::1");
    test_output.str("");

    nodeinfo.add_address(address2);
    test_output << nodeinfo;
    EXPECT_EQ(test_output.str(), "NodeInfo: hoplimit=64 fd00::1 fd00::2");
    test_output.str("");

    nodeinfo.set_hoplimit(30);
    test_output << nodeinfo;
    EXPECT_EQ(test_output.str(), "NodeInfo: hoplimit=30 fd00::1 fd00::2");
    test_output.str("");

    nodeinfo.set_mac_address(
        Tins::HWAddress<6>(std::string("52:54:00:b2:fa:7f")));
    test_output << nodeinfo;
    EXPECT_EQ(test_output.str(),
              "NodeInfo: hoplimit=30 fd00::1 fd00::2 52:54:00:b2:fa:7f");
}
