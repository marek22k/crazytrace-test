#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <tins/tins.h>
#include "nodecontainer.hpp"
#include "nodeinfo.hpp"

class NodeContainerTest : public testing::Test
{
    protected:
        // cppcheck-suppress duplInheritedMember
        static void SetUpTestSuite()
        {
            testing::Test::SetUpTestSuite();

            /* Container 1 */
            container1 = std::make_unique<NodeContainer>();

            c1_child_node1 = std::make_shared<NodeInfo>();
            c1_child_node1->set_hoplimit(20);
            c1_child_node1->set_mac_address(
                Tins::HWAddress<6>("52:54:00:b2:fa:7f"));
            c1_child_node1->add_address(Tins::IPv6Address("fd00::11"));
            c1_child_node1->add_address(Tins::IPv6Address("fd00::12"));
            container1->add_node(c1_child_node1);

            c1_child_node2 = std::make_shared<NodeInfo>();
            c1_child_node2->set_hoplimit(30);
            c1_child_node2->set_mac_address(
                Tins::HWAddress<6>("52:54:00:b2:fa:7e"));
            c1_child_node2->add_address(Tins::IPv6Address("fd00::21"));
            container1->add_node(c1_child_node2);

            c1_child_node3 = std::make_shared<NodeInfo>();
            c1_child_node3->set_mac_address(
                Tins::HWAddress<6>("52:54:00:b2:fa:7d"));
            c1_child_node3->add_address(Tins::IPv6Address("fd00::3"));

            c1_child_node3_child1 = std::make_shared<NodeInfo>();
            c1_child_node3_child1->add_address(Tins::IPv6Address("fd00::3:1"));
            c1_child_node3->add_node(c1_child_node3_child1);

            c1_child_node3_child2 = std::make_shared<NodeInfo>();
            c1_child_node3_child2->add_address(Tins::IPv6Address("fd00::3:2"));

            c1_child_node3_child2_child1 = std::make_shared<NodeInfo>();
            c1_child_node3_child2_child1->add_address(
                Tins::IPv6Address("fd00::3:2:1"));
            c1_child_node3_child2->add_node(c1_child_node3_child2_child1);

            c1_child_node3->add_node(c1_child_node3_child2);
            container1->add_node(c1_child_node3);

            /* Container 2 */
            container2 = std::make_unique<NodeContainer>();

            c2_child_node1 = std::make_shared<NodeInfo>();
            c2_child_node1->set_mac_address(
                Tins::HWAddress<6>("52:54:00:b2:fa:7f"));
            c2_child_node1->add_address(Tins::IPv6Address("fd01::"));
            container2->add_node(c2_child_node1);

            /* Container 3 */
            container3 = std::make_unique<NodeContainer>();
        }

        Tins::EthernetII
            create_echo_request(const Tins::HWAddress<6>& source_mac,
                                const Tins::HWAddress<6>& destination_mac,
                                const Tins::IPv6Address& source_address,
                                const Tins::IPv6Address& destination_address,
                                const int hoplimit,
                                const int icmp_identifier,
                                const int icmp_sequence,
                                const Tins::RawPDU::payload_type& payload)
        {
            Tins::EthernetII packet =
                Tins::EthernetII(destination_mac, source_mac) /
                Tins::IPv6(destination_address, source_address) /
                Tins::ICMPv6(Tins::ICMPv6::Types::ECHO_REQUEST);
            Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
            inner_ipv6.hop_limit(hoplimit);
            Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
            inner_icmpv6.identifier(icmp_identifier);
            inner_icmpv6.sequence(icmp_sequence);
            inner_icmpv6.inner_pdu(Tins::RawPDU(payload));

            const Tins::PDU::serialization_type serialized_packet =
                packet.serialize();
            const Tins::EthernetII final_packet(serialized_packet.data(),
                                                serialized_packet.size());

            return final_packet;
        }

        std::string
            create_echo_reply(const Tins::HWAddress<6>& source_mac,
                              const Tins::HWAddress<6>& destination_mac,
                              const Tins::IPv6Address& source_address,
                              const Tins::IPv6Address& destination_address,
                              const int hoplimit,
                              const int icmp_identifier,
                              const int icmp_sequence,
                              const Tins::RawPDU::payload_type& payload)
        {
            Tins::EthernetII packet =
                Tins::EthernetII(destination_mac, source_mac) /
                Tins::IPv6(destination_address, source_address) /
                Tins::ICMPv6(Tins::ICMPv6::Types::ECHO_REPLY);
            Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
            inner_ipv6.hop_limit(hoplimit);
            Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
            inner_icmpv6.identifier(icmp_identifier);
            inner_icmpv6.sequence(icmp_sequence);
            inner_icmpv6.inner_pdu(Tins::RawPDU(payload));

            const Tins::PDU::serialization_type serialized_packet =
                packet.serialize();
            const std::string final_packet(serialized_packet.begin(),
                                           serialized_packet.end());

            return final_packet;
        }

        std::string create_time_exceeded_echo_request(const Tins::HWAddress<6>& source_mac,
                              const Tins::HWAddress<6>& destination_mac,
                              const Tins::IPv6Address& source_address,
                              const Tins::IPv6Address& destination_address,
                              const int hoplimit,
                              Tins::EthernetII& echo_request)
                              {
                    Tins::EthernetII packet =
                        Tins::EthernetII(destination_mac,
                                         source_mac) /
                        Tins::IPv6(destination_address,
                                   source_address) /
                        Tins::ICMPv6(Tins::ICMPv6::Types::TIME_EXCEEDED);
                    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
                    inner_ipv6.hop_limit(hoplimit);
                    Tins::ICMPv6& inner_icmpv6 =
                        inner_ipv6.rfind_pdu<Tins::ICMPv6>();
                    inner_icmpv6.inner_pdu(
                        Tins::RawPDU(echo_request.rfind_pdu<Tins::IPv6>().serialize()));

            const Tins::PDU::serialization_type serialized_packet =
                packet.serialize();
            const std::string final_packet(serialized_packet.begin(),
                                           serialized_packet.end());

            return final_packet;
                              }

        // NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
        static std::unique_ptr<NodeContainer> container1;
        static std::shared_ptr<NodeInfo> c1_child_node1;
        static std::shared_ptr<NodeInfo> c1_child_node2;
        static std::shared_ptr<NodeInfo> c1_child_node3;
        static std::shared_ptr<NodeInfo> c1_child_node3_child1;
        static std::shared_ptr<NodeInfo> c1_child_node3_child2;
        static std::shared_ptr<NodeInfo> c1_child_node3_child2_child1;
        static std::unique_ptr<NodeContainer> container2;
        static std::shared_ptr<NodeInfo> c2_child_node1;
        static std::unique_ptr<NodeContainer> container3;
        // NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)
};

// NOLINTBEGIN(cppcoreguidelines-avoid-non-const-global-variables)
std::unique_ptr<NodeContainer> NodeContainerTest::container1;
std::shared_ptr<NodeInfo> NodeContainerTest::c1_child_node1;
std::shared_ptr<NodeInfo> NodeContainerTest::c1_child_node2;
std::shared_ptr<NodeInfo> NodeContainerTest::c1_child_node3;
std::shared_ptr<NodeInfo> NodeContainerTest::c1_child_node3_child1;
std::shared_ptr<NodeInfo> NodeContainerTest::c1_child_node3_child2;
std::shared_ptr<NodeInfo> NodeContainerTest::c1_child_node3_child2_child1;
std::unique_ptr<NodeContainer> NodeContainerTest::container2;
std::shared_ptr<NodeInfo> NodeContainerTest::c2_child_node1;
std::unique_ptr<NodeContainer> NodeContainerTest::container3;

// NOLINTEND(cppcoreguidelines-avoid-non-const-global-variables)

TEST_F(NodeContainerTest, Print)
{
    std::ostringstream test_output;

    container1->print(test_output);
    EXPECT_EQ(
        test_output.str(),
        "NodeContainer: 3 childnodes\nChilds:\n\tNodeInfo: hoplimit=20 "
        "fd00::11 fd00::12 52:54:00:b2:fa:7f\n\tNodeInfo: hoplimit=30 fd00::21 "
        "52:54:00:b2:fa:7e\n\tNodeInfo: hoplimit=64 fd00::3 "
        "52:54:00:b2:fa:7d\n\tChilds:\n\t\tNodeInfo: hoplimit=64 "
        "fd00::3:1\n\t\tNodeInfo: hoplimit=64 "
        "fd00::3:2\n\t\tChilds:\n\t\t\tNodeInfo: hoplimit=64 fd00::3:2:1\n");
    test_output.str("");

    container2->print(test_output);
    EXPECT_EQ(test_output.str(),
              "NodeContainer: 1 childnodes\nChilds:\n\tNodeInfo: hoplimit=64 "
              "fd01:: 52:54:00:b2:fa:7f\n");
    test_output.str("");

    container3->print(test_output);
    EXPECT_EQ(test_output.str(), "NodeContainer: 0 childnodes\n");
}

TEST_F(NodeContainerTest, Output)
{
    std::ostringstream test_output;

    test_output << *container1;
    EXPECT_EQ(test_output.str(), "NodeContainer: 3 childnodes");
    test_output.str("");

    test_output << *container2;
    EXPECT_EQ(test_output.str(), "NodeContainer: 1 childnodes");
    test_output.str("");

    test_output << *container3;
    EXPECT_EQ(test_output.str(), "NodeContainer: 0 childnodes");
}

TEST_F(NodeContainerTest, MaxDepth)
{
    EXPECT_EQ(container1->max_depth(), 3);
    EXPECT_EQ(container2->max_depth(), 1);
    EXPECT_EQ(container3->max_depth(), 0);
}

TEST_F(NodeContainerTest, Comparison)
{
    NodeContainer container;

    auto c1_child_node1 = std::make_shared<NodeInfo>();
    c1_child_node1->set_hoplimit(20);
    c1_child_node1->set_mac_address(Tins::HWAddress<6>("52:54:00:b2:fa:7f"));
    c1_child_node1->add_address(Tins::IPv6Address("fd00::11"));
    c1_child_node1->add_address(Tins::IPv6Address("fd00::12"));
    container.add_node(c1_child_node1);

    auto c1_child_node2 = std::make_shared<NodeInfo>();
    c1_child_node2->set_hoplimit(30);
    c1_child_node2->set_mac_address(Tins::HWAddress<6>("52:54:00:b2:fa:7e"));
    c1_child_node2->add_address(Tins::IPv6Address("fd00::21"));
    container.add_node(c1_child_node2);

    auto c1_child_node3 = std::make_shared<NodeInfo>();
    c1_child_node3->set_mac_address(Tins::HWAddress<6>("52:54:00:b2:fa:7d"));
    c1_child_node3->add_address(Tins::IPv6Address("fd00::3"));

    auto c1_child_node3_child1 = std::make_shared<NodeInfo>();
    c1_child_node3_child1->add_address(Tins::IPv6Address("fd00::3:1"));
    c1_child_node3->add_node(c1_child_node3_child1);

    auto c1_child_node3_child2 = std::make_shared<NodeInfo>();
    c1_child_node3_child2->add_address(Tins::IPv6Address("fd00::3:2"));

    auto c1_child_node3_child2_child1 = std::make_shared<NodeInfo>();
    c1_child_node3_child2_child1->add_address(Tins::IPv6Address("fd00::3:2:1"));
    c1_child_node3_child2->add_node(c1_child_node3_child2_child1);

    c1_child_node3->add_node(c1_child_node3_child2);
    container.add_node(c1_child_node3);

    EXPECT_EQ(*container1, container);
    EXPECT_NE(*container2, container);
    EXPECT_NE(*container3, container);
}

TEST_F(NodeContainerTest, GetReplyEchoRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address destination_address("fd00::3:2:1");
    constexpr int hoplimit = 55;
    constexpr int icmp_identifier = 56;
    constexpr int icmp_sequence = 1;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    const Tins::EthernetII request_packet =
        create_echo_request(source_mac,
                            destination_mac,
                            source_address,
                            destination_address,
                            hoplimit,
                            icmp_identifier,
                            icmp_sequence,
                            payload);

    const NodeRequest echo_request(request_packet);
    NodeReply echo_reply = container1->get_reply(echo_request);

    const std::string reply_packet = create_echo_reply(destination_mac,
                                                       source_mac,
                                                       destination_address,
                                                       source_address,
                                                       62,
                                                       icmp_identifier,
                                                       icmp_sequence,
                                                       payload);
    ASSERT_EQ(echo_reply.get_type(), NodeReplyType::ICMP_ECHO_REPLY);
    EXPECT_EQ(echo_reply.to_packet(), reply_packet);
}

TEST_F(NodeContainerTest, GetReplyEchoRequestTimeExceeded)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address destination_address("fd00::3:2:1");
    constexpr int hoplimit = 1;
    constexpr int icmp_identifier = 56;
    constexpr int icmp_sequence = 1;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    Tins::EthernetII request_packet =
        create_echo_request(source_mac,
                            destination_mac,
                            source_address,
                            destination_address,
                            hoplimit,
                            icmp_identifier,
                            icmp_sequence,
                            payload);

    const NodeRequest echo_request(request_packet);
    NodeReply echo_reply = container1->get_reply(echo_request);

    const Tins::IPv6Address time_exceeded_hop("fd00::3");
    const std::string reply_packet = create_time_exceeded_echo_request(destination_mac,
                                                       source_mac,
                                                       time_exceeded_hop,
                                                       source_address,
                                                       64,
                                                       request_packet);
    ASSERT_EQ(echo_reply.get_type(), NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST);
    EXPECT_EQ(echo_reply.to_packet(), reply_packet);
}
