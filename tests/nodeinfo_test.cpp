#include <memory>
#include <sstream>
#include <tins/tins.h>
#include "nodeinfo.hpp"
#include <gtest/gtest.h>

TEST(nodeinfo_test, mac_address)
{
    Tins::HWAddress<6> mac_address(std::string("52:54:00:b2:fa:7f"));
    NodeInfo nodeinfo;
    nodeinfo.set_mac_address(mac_address);
    EXPECT_EQ(mac_address, nodeinfo.get_mac_address());
    EXPECT_EQ(mac_address.to_string(), nodeinfo.get_mac_address().to_string());
}

TEST(nodeinfo_test, hop_limit)
{
    constexpr int hoplimit = 150;
    NodeInfo nodeinfo;
    EXPECT_EQ(64, nodeinfo.get_hoplimit());

    nodeinfo.set_hoplimit(hoplimit);
    EXPECT_EQ(hoplimit, nodeinfo.get_hoplimit());
}

TEST(nodeinfo_test, ipv6_address)
{
    Tins::IPv6Address address1(std::string("fd00::1"));
    Tins::IPv6Address address2(std::string("fd00::2"));
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

TEST(nodeinfo_test, has_address)
{
    Tins::IPv6Address address1(std::string("fd00::1"));
    Tins::IPv6Address address2(std::string("fd00::2"));
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

TEST(nodeinfo_test, output)
{
    Tins::IPv6Address address1(std::string("fd00::1"));
    Tins::IPv6Address address2(std::string("fd00::2"));
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

TEST(nodeinfo_test, childnodes)
{
    /* Creating the test objects */
    NodeInfo root_node;
    root_node.set_mac_address(
        Tins::HWAddress<6>(std::string("52:54:00:b2:fa:7f")));
    root_node.add_address(Tins::IPv6Address(std::string("fd00::")));

    std::shared_ptr<NodeInfo> child_node1 = std::make_shared<NodeInfo>();
    child_node1->set_hoplimit(20);
    child_node1->add_address(Tins::IPv6Address(std::string("fd00::11")));
    child_node1->add_address(Tins::IPv6Address(std::string("fd00::12")));
    root_node.add_node(child_node1);

    std::shared_ptr<NodeInfo> child_node2 = std::make_shared<NodeInfo>();
    child_node2->set_hoplimit(30);
    child_node2->add_address(Tins::IPv6Address(std::string("fd00::21")));
    root_node.add_node(child_node2);

    std::shared_ptr<NodeInfo> child_node3 = std::make_shared<NodeInfo>();
    child_node3->add_address(Tins::IPv6Address(std::string("fd00::3")));

    std::shared_ptr<NodeInfo> child_node3_child1 = std::make_shared<NodeInfo>();
    child_node3_child1->add_address(
        Tins::IPv6Address(std::string("fd00::3:1")));
    child_node3->add_node(child_node3_child1);

    std::shared_ptr<NodeInfo> child_node3_child2 = std::make_shared<NodeInfo>();
    child_node3_child2->add_address(
        Tins::IPv6Address(std::string("fd00::3:2")));

    std::shared_ptr<NodeInfo> child_node3_child2_child1 =
        std::make_shared<NodeInfo>();
    child_node3_child2_child1->add_address(
        Tins::IPv6Address(std::string("fd00::3:2:1")));
    child_node3_child2->add_node(child_node3_child2_child1);

    child_node3->add_node(child_node3_child2);
    root_node.add_node(child_node3);

    /* Testing the print function */
    std::ostringstream test_output;

    test_output << root_node;
    EXPECT_EQ(test_output.str(),
              "NodeInfo: hoplimit=64 fd00:: 52:54:00:b2:fa:7f");
    test_output.str("");

    root_node.print(test_output);
    EXPECT_EQ(
        test_output.str(),
        "NodeInfo: hoplimit=64 fd00:: 52:54:00:b2:fa:7f\nChilds:\n\tNodeInfo: "
        "hoplimit=64 fd00::3\n\tChilds:\n\t\tNodeInfo: hoplimit=64 "
        "fd00::3:2\n\t\tChilds:\n\t\t\tNodeInfo: hoplimit=64 "
        "fd00::3:2:1\n\t\tNodeInfo: hoplimit=64 fd00::3:1\n\tNodeInfo: "
        "hoplimit=30 fd00::21\n\tNodeInfo: hoplimit=20 fd00::11 fd00::12\n");
    test_output.str("");
    
    child_node3_child2->print(test_output);
    EXPECT_EQ(test_output.str(), "NodeInfo: hoplimit=64 fd00::3:2\nChilds:\n\tNodeInfo: hoplimit=64 fd00::3:2:1\n");

    /* Testing the max depth function */
    EXPECT_EQ(root_node.max_depth(), 4);
    EXPECT_EQ(child_node3_child2->max_depth(), 2);
    EXPECT_EQ(child_node1->max_depth(), 1);

    /* Testing the route to function */
    auto route1 = child_node3->get_route_to(Tins::IPv6Address(std::string("fd00::3:2:1")));
    EXPECT_EQ(route1.size(), 2);
    EXPECT_EQ(route1[1], child_node3_child2);
    EXPECT_EQ(route1[0], child_node3_child2_child1);

    auto route2 = child_node1->get_route_to(Tins::IPv6Address(std::string("fd00::12")));
    EXPECT_EQ(route2.size(), 0);

    auto route3 = root_node.get_route_to(Tins::IPv6Address(std::string("fd00::12")));
    EXPECT_EQ(route3.size(), 1);
    EXPECT_EQ(route3[0], child_node1);
}
