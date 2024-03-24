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

        Tins::EthernetII
            create_udp_request(const Tins::HWAddress<6>& source_mac,
                               const Tins::HWAddress<6>& destination_mac,
                               const Tins::IPv6Address& source_address,
                               const Tins::IPv6Address& destination_address,
                               const int hoplimit,
                               const int udp_dport,
                               const int udp_sport,
                               const Tins::RawPDU::payload_type& payload)
        {
            Tins::EthernetII packet =
                Tins::EthernetII(destination_mac, source_mac) /
                Tins::IPv6(destination_address, source_address) /
                Tins::UDP(udp_dport, udp_sport);
            Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
            inner_ipv6.hop_limit(hoplimit);
            Tins::UDP& inner_udp = inner_ipv6.rfind_pdu<Tins::UDP>();
            inner_udp.inner_pdu(Tins::RawPDU(payload));

            const Tins::PDU::serialization_type serialized_packet =
                packet.serialize();
            const Tins::EthernetII final_packet(serialized_packet.data(),
                                                serialized_packet.size());

            return final_packet;
        }

        Tins::EthernetII
            create_ndp_request(const Tins::HWAddress<6>& source_mac,
                               const Tins::IPv6Address& source_address,
                               const Tins::IPv6Address& target_address,
                               const int hoplimit)
        {
            Tins::EthernetII packet =
                Tins::EthernetII(Tins::HWAddress<6>("33:33:ff:48:b2:ae"),
                                 source_mac) /
                Tins::IPv6(Tins::IPv6Address("ff02::1:ff48:b2ae"),
                           source_address) /
                Tins::ICMPv6(Tins::ICMPv6::Types::NEIGHBOUR_SOLICIT);
            Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
            inner_ipv6.hop_limit(hoplimit);
            Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
            inner_icmpv6.target_addr(target_address);

            const Tins::PDU::serialization_type serialized_packet =
                packet.serialize();
            const Tins::EthernetII final_packet(serialized_packet.data(),
                                                serialized_packet.size());

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

TEST_F(NodeContainerTest, GetReplyForEchoRequest)
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
    const NodeReply echo_reply = container1->get_reply(echo_request);

    NodeReply expected_reply(NodeReplyType::ICMP_ECHO_REPLY,
                             source_mac,
                             source_address,
                             destination_mac,
                             destination_address);
    expected_reply.icmp_echo_reply(icmp_identifier, icmp_sequence, payload);
    expected_reply.set_hoplimit(62);
    EXPECT_EQ(echo_reply, expected_reply);
}

TEST_F(NodeContainerTest, GetReplyTimeExceededForEchoRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address destination_address("fd00::3:2:1");
    constexpr int hoplimit = 1;
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
    const NodeReply echo_reply = container1->get_reply(echo_request);

    const Tins::IPv6Address time_exceeded_hop("fd00::3");

    NodeReply expected_reply(
        NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST,
        source_mac,
        source_address,
        destination_mac,
        time_exceeded_hop);
    expected_reply.icmp_echo_reply(icmp_identifier, icmp_sequence, payload);
    expected_reply.packet_reassembly(destination_address);
    expected_reply.set_hoplimit(64);
    EXPECT_EQ(echo_reply, expected_reply);
}

TEST_F(NodeContainerTest, GetReplyForUdpRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address destination_address("fd00::3");
    constexpr int hoplimit = 5;
    constexpr int udp_dport = 33434;
    constexpr int udp_sport = 16383;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    const Tins::EthernetII request_packet =
        create_udp_request(source_mac,
                           destination_mac,
                           source_address,
                           destination_address,
                           hoplimit,
                           udp_dport,
                           udp_sport,
                           payload);

    const NodeRequest echo_request(request_packet);
    const NodeReply echo_reply = container1->get_reply(echo_request);

    NodeReply expected_reply(NodeReplyType::ICMP_PORT_UNREACHABLE,
                             source_mac,
                             source_address,
                             destination_mac,
                             destination_address);
    expected_reply.udp_response(payload, udp_dport, udp_sport);
    expected_reply.packet_reassembly(destination_address);
    expected_reply.set_hoplimit(64);
    EXPECT_EQ(echo_reply, expected_reply);
}

TEST_F(NodeContainerTest, GetTimeExceededReplyForUdpRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address destination_address("fd00::3:2:1");
    constexpr int hoplimit = 1;
    constexpr int udp_dport = 33434;
    constexpr int udp_sport = 16383;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    const Tins::EthernetII request_packet =
        create_udp_request(source_mac,
                           destination_mac,
                           source_address,
                           destination_address,
                           hoplimit,
                           udp_dport,
                           udp_sport,
                           payload);

    const NodeRequest echo_request(request_packet);
    const NodeReply echo_reply = container1->get_reply(echo_request);

    const Tins::IPv6Address time_exceeded_hop("fd00::3");

    NodeReply expected_reply(NodeReplyType::ICMP_TIME_EXCEEDED_UDP,
                             source_mac,
                             source_address,
                             destination_mac,
                             time_exceeded_hop);
    expected_reply.udp_response(payload, udp_dport, udp_sport);
    expected_reply.packet_reassembly(destination_address);
    expected_reply.set_hoplimit(64);
    EXPECT_EQ(echo_reply, expected_reply);
}

TEST_F(NodeContainerTest, GetNoReplyForNonExistAddress)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address destination_address("fd00::3:2:1");
    constexpr int hoplimit = 1;
    constexpr int udp_dport = 33434;
    constexpr int udp_sport = 16383;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    const Tins::EthernetII request_packet =
        create_udp_request(source_mac,
                           destination_mac,
                           source_address,
                           destination_address,
                           hoplimit,
                           udp_dport,
                           udp_sport,
                           payload);

    const NodeRequest echo_request(request_packet);
    const NodeReply echo_reply = container2->get_reply(echo_request);

    const Tins::IPv6Address time_exceeded_hop("fd00::3");

    const NodeReply expected_reply(NodeReplyType::NOREPLY);
    EXPECT_EQ(echo_reply, expected_reply);
}

TEST_F(NodeContainerTest, GetNoReplyForUnknownPacket)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7d");
    const Tins::IPv4Address source_address("10.10.10.10");
    const Tins::IPv4Address destination_address("10.10.10.11");

    Tins::EthernetII packet = Tins::EthernetII(destination_mac, source_mac) /
                              Tins::IP(destination_address, source_address);

    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const Tins::EthernetII final_packet(serialized_packet.data(),
                                        serialized_packet.size());

    const NodeRequest echo_request(final_packet);
    const NodeReply echo_reply = container1->get_reply(echo_request);

    const NodeReply expected_reply(NodeReplyType::NOREPLY);
    EXPECT_EQ(echo_reply, expected_reply);
}

TEST_F(NodeContainerTest, GetReplyForNdpRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> target_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address target_address("fd00::3");
    constexpr int hoplimit = 5;

    const Tins::EthernetII request_packet = create_ndp_request(
        source_mac, source_address, target_address, hoplimit);

    const NodeRequest echo_request(request_packet);
    const NodeReply echo_reply = container1->get_reply(echo_request);

    const NodeReply expected_reply(NodeReplyType::ICMP_NDP,
                                   source_mac,
                                   source_address,
                                   target_mac,
                                   target_address);
    EXPECT_EQ(echo_reply, expected_reply);
}

TEST_F(NodeContainerTest, GetNoReplyForNdpRequestDueToHoplimit)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> target_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address target_address("fd00::3");
    constexpr int hoplimit = 0;

    const Tins::EthernetII request_packet = create_ndp_request(
        source_mac, source_address, target_address, hoplimit);

    const NodeRequest echo_request(request_packet);
    const NodeReply echo_reply = container1->get_reply(echo_request);

    const NodeReply expected_reply(NodeReplyType::NOREPLY);
    EXPECT_EQ(echo_reply, expected_reply);
}

TEST_F(NodeContainerTest, GetNoReplyForNdpRequestDueToNonExistTarget)
{
    const Tins::HWAddress<6> source_mac("52:54:01:b2:fa:7f");
    const Tins::HWAddress<6> target_mac("52:54:00:b2:fa:7d");
    const Tins::IPv6Address source_address("fd01::1");
    const Tins::IPv6Address target_address("fd00::3");
    constexpr int hoplimit = 5;

    const Tins::EthernetII request_packet = create_ndp_request(
        source_mac, source_address, target_address, hoplimit);

    const NodeRequest echo_request(request_packet);
    const NodeReply echo_reply = container2->get_reply(echo_request);

    const NodeReply expected_reply(NodeReplyType::NOREPLY);
    EXPECT_EQ(echo_reply, expected_reply);
}
