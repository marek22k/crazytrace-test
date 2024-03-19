#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <tins/tins.h>
#include "nodeinfo.hpp"

class nodeinfo_children_test : public testing::Test
{
    protected:
        // cppcheck-suppress duplInheritedMember
        static void SetUpTestSuite()
        {
            testing::Test::SetUpTestSuite();

            root_node = std::make_unique<NodeInfo>();
            root_node->set_mac_address(
                Tins::HWAddress<6>(std::string("52:54:00:b2:fa:7f")));
            root_node->add_address(Tins::IPv6Address(std::string("fd00::")));

            child_node1 = std::make_shared<NodeInfo>();
            child_node1->set_hoplimit(20);
            child_node1->add_address(
                Tins::IPv6Address(std::string("fd00::11")));
            child_node1->add_address(
                Tins::IPv6Address(std::string("fd00::12")));
            root_node->add_node(child_node1);

            child_node2 = std::make_shared<NodeInfo>();
            child_node2->set_hoplimit(30);
            child_node2->add_address(
                Tins::IPv6Address(std::string("fd00::21")));
            root_node->add_node(child_node2);

            child_node3 = std::make_shared<NodeInfo>();
            child_node3->add_address(Tins::IPv6Address(std::string("fd00::3")));

            child_node3_child1 = std::make_shared<NodeInfo>();
            child_node3_child1->add_address(
                Tins::IPv6Address(std::string("fd00::3:1")));
            child_node3->add_node(child_node3_child1);

            child_node3_child2 = std::make_shared<NodeInfo>();
            child_node3_child2->add_address(
                Tins::IPv6Address(std::string("fd00::3:2")));

            child_node3_child2_child1 = std::make_shared<NodeInfo>();
            child_node3_child2_child1->add_address(
                Tins::IPv6Address(std::string("fd00::3:2:1")));
            child_node3_child2->add_node(child_node3_child2_child1);

            child_node3->add_node(child_node3_child2);
            root_node->add_node(child_node3);
        }

        // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
        static std::unique_ptr<NodeInfo> root_node;
        static std::shared_ptr<NodeInfo> child_node1;
        static std::shared_ptr<NodeInfo> child_node2;
        static std::shared_ptr<NodeInfo> child_node3;
        static std::shared_ptr<NodeInfo> child_node3_child1;
        static std::shared_ptr<NodeInfo> child_node3_child2;
        static std::shared_ptr<NodeInfo> child_node3_child2_child1;
        // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
};

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
std::unique_ptr<NodeInfo> nodeinfo_children_test::root_node;
std::shared_ptr<NodeInfo> nodeinfo_children_test::child_node1;
std::shared_ptr<NodeInfo> nodeinfo_children_test::child_node2;
std::shared_ptr<NodeInfo> nodeinfo_children_test::child_node3;
std::shared_ptr<NodeInfo> nodeinfo_children_test::child_node3_child1;
std::shared_ptr<NodeInfo> nodeinfo_children_test::child_node3_child2;
std::shared_ptr<NodeInfo> nodeinfo_children_test::child_node3_child2_child1;

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

TEST_F(nodeinfo_children_test, print)
{
    /* Testing the print function */
    std::ostringstream test_output;

    test_output << *root_node;
    EXPECT_EQ(test_output.str(),
              "NodeInfo: hoplimit=64 fd00:: 52:54:00:b2:fa:7f");
    test_output.str("");

    root_node->print(test_output);
    EXPECT_EQ(
        test_output.str(),
        "NodeInfo: hoplimit=64 fd00:: 52:54:00:b2:fa:7f\nChilds:\n\tNodeInfo: "
        "hoplimit=64 fd00::3\n\tChilds:\n\t\tNodeInfo: hoplimit=64 "
        "fd00::3:2\n\t\tChilds:\n\t\t\tNodeInfo: hoplimit=64 "
        "fd00::3:2:1\n\t\tNodeInfo: hoplimit=64 fd00::3:1\n\tNodeInfo: "
        "hoplimit=30 fd00::21\n\tNodeInfo: hoplimit=20 fd00::11 fd00::12\n");
    test_output.str("");

    child_node3_child2->print(test_output);
    EXPECT_EQ(test_output.str(),
              "NodeInfo: hoplimit=64 fd00::3:2\nChilds:\n\tNodeInfo: "
              "hoplimit=64 fd00::3:2:1\n");
}

TEST_F(nodeinfo_children_test, max_depth)
{
    EXPECT_EQ(root_node->max_depth(), 4);
    EXPECT_EQ(child_node3_child2->max_depth(), 2);
    EXPECT_EQ(child_node1->max_depth(), 1);
}

TEST_F(nodeinfo_children_test, get_route_to)
{
    auto route1 = child_node3->get_route_to(
        Tins::IPv6Address(std::string("fd00::3:2:1")));
    EXPECT_EQ(route1.size(), 2);
    EXPECT_EQ(route1[1], child_node3_child2);
    EXPECT_EQ(route1[0], child_node3_child2_child1);

    auto route2 =
        child_node1->get_route_to(Tins::IPv6Address(std::string("fd00::12")));
    EXPECT_EQ(route2.size(), 0);

    auto route3 =
        root_node->get_route_to(Tins::IPv6Address(std::string("fd00::12")));
    EXPECT_EQ(route3.size(), 1);
    EXPECT_EQ(route3[0], child_node1);
}
