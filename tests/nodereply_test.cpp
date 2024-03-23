#include <gtest/gtest.h>
#include <stdexcept>
#include <tins/tins.h>
#include "nodereply.hpp"

TEST(NodeReplyTest, SimpleInit)
{
    const NodeReply reply(NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST);
    EXPECT_EQ(reply.get_type(),
              NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST);
}

TEST(NodeReplyTest, NoReply)
{
    NodeReply reply(NodeReplyType::NOREPLY);
    EXPECT_EQ(reply.get_type(), NodeReplyType::NOREPLY);

    try
    {
        [[maybe_unused]] const std::string reply_packet = reply.to_packet();
        FAIL();
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "Attempt to create a packet, although there is no reply.");
    }

    try
    {
        reply.icmp_echo_reply(56, 1, {});
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require ICMP echo reply "
                  "information.");
    }

    try
    {
        reply.udp_response({}, 33434, 45834);
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require UDP information.");
    }

    try
    {
        reply.packet_reassembly(Tins::IPv6Address("fd00::"));
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require original "
                  "destionation address information.");
    }

    std::ostringstream test_output;
    test_output << reply;
    EXPECT_EQ(test_output.str(), "NOREPLY");
}

TEST(NodeReplyTest, IcmpEchoReply)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7e");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("fd00::2");
    constexpr int hoplimit = 55;
    constexpr int icmp_identifier = 56;
    constexpr int icmp_sequence = 1;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    /* Reply */
    NodeReply reply(NodeReplyType::ICMP_ECHO_REPLY,
                    destination_mac,
                    destination_address,
                    source_mac,
                    source_address);
    reply.set_hoplimit(hoplimit);
    reply.icmp_echo_reply(icmp_identifier, icmp_sequence, payload);
    const std::string actual_packet = reply.to_packet();

    /* Expected packet */
    Tins::EthernetII packet = Tins::EthernetII(destination_mac, source_mac) /
                              Tins::IPv6(destination_address, source_address) /
                              Tins::ICMPv6(Tins::ICMPv6::Types::ECHO_REPLY);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(hoplimit);
    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
    inner_icmpv6.identifier(icmp_identifier);
    inner_icmpv6.sequence(icmp_sequence);
    inner_icmpv6.inner_pdu(Tins::RawPDU(payload));

    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const std::string expected_packet(serialized_packet.begin(),
                                      serialized_packet.end());

    /* Tests */
    try
    {
        reply.udp_response({}, 33434, 45834);
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require UDP information.");
    }

    try
    {
        reply.packet_reassembly(Tins::IPv6Address("fd00::"));
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require original "
                  "destionation address information.");
    }

    std::ostringstream test_output;
    test_output << reply;
    EXPECT_EQ(test_output.str(),
              "REPLY ICMP_ECHO_REPLY: fd00::1 (52:54:00:b2:fa:7f) -> fd00::2 "
              "(52:54:00:b2:fa:7e) Hoplimit=55: ID=56 SEQ=1 Payload: 08 04 05 "
              "09 ff 00 00 00 00 00");

    EXPECT_EQ(actual_packet, expected_packet);
    EXPECT_EQ(reply.get_type(), NodeReplyType::ICMP_ECHO_REPLY);
}

TEST(NodeReplyTest, IcmpTimeExceededIcmpEchoRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7e");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("fd00::2");
    const Tins::IPv6Address original_destination_address("fd00::3");
    constexpr int hoplimit = 55;
    constexpr int icmp_identifier = 56;
    constexpr int icmp_sequence = 1;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    /* Reply */
    NodeReply reply(NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST,
                    destination_mac,
                    destination_address,
                    source_mac,
                    source_address);
    reply.set_hoplimit(hoplimit);
    reply.icmp_echo_reply(icmp_identifier, icmp_sequence, payload);
    reply.packet_reassembly(original_destination_address);
    const std::string actual_packet = reply.to_packet();

    /* Expected packet */
    Tins::IPv6 embedded_packet =
        Tins::IPv6(original_destination_address, destination_address) /
        Tins::ICMPv6(Tins::ICMPv6::Types::ECHO_REQUEST);
    embedded_packet.hop_limit(1);
    Tins::ICMPv6& embedded_inner_icmpv6 =
        embedded_packet.rfind_pdu<Tins::ICMPv6>();
    embedded_inner_icmpv6.identifier(icmp_identifier);
    embedded_inner_icmpv6.sequence(icmp_sequence);
    embedded_inner_icmpv6.inner_pdu(Tins::RawPDU(payload));
    Tins::PDU::serialization_type serialized_embedded_packet =
        embedded_packet.serialize();

    Tins::EthernetII packet = Tins::EthernetII(destination_mac, source_mac) /
                              Tins::IPv6(destination_address, source_address) /
                              Tins::ICMPv6(Tins::ICMPv6::Types::TIME_EXCEEDED);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(hoplimit);
    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
    inner_icmpv6.inner_pdu(Tins::RawPDU(serialized_embedded_packet));

    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const std::string expected_packet(serialized_packet.begin(),
                                      serialized_packet.end());

    /* Tests */
    try
    {
        reply.udp_response({}, 33434, 45834);
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require UDP information.");
    }

    std::ostringstream test_output;
    test_output << reply;
    EXPECT_EQ(test_output.str(),
              "REPLY ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST: fd00::1 "
              "(52:54:00:b2:fa:7f) -> fd00::2 (52:54:00:b2:fa:7e) Hoplimit=55 "
              "LENGTH=10");

    EXPECT_EQ(actual_packet, expected_packet);
    EXPECT_EQ(reply.get_type(),
              NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST);
}

TEST(NodeReplyTest, IcmpPortUnreachable)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7e");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("fd00::2");
    const Tins::IPv6Address original_destination_address("fd00::3");
    constexpr int hoplimit = 55;
    constexpr int udp_sport = 46432;
    constexpr int udp_dport = 34344;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    /* Reply */
    NodeReply reply(NodeReplyType::ICMP_PORT_UNREACHABLE,
                    destination_mac,
                    destination_address,
                    source_mac,
                    source_address);
    reply.set_hoplimit(hoplimit);
    reply.udp_response(payload, udp_dport, udp_sport);
    reply.packet_reassembly(original_destination_address);
    const std::string actual_packet = reply.to_packet();

    /* Expected packet */
    Tins::IPv6 embedded_packet =
        Tins::IPv6(original_destination_address, destination_address) /
        Tins::UDP(udp_dport, udp_sport);
    embedded_packet.hop_limit(1);
    Tins::UDP& embedded_inner_udp = embedded_packet.rfind_pdu<Tins::UDP>();
    embedded_inner_udp.inner_pdu(Tins::RawPDU(payload));
    Tins::PDU::serialization_type serialized_embedded_packet =
        embedded_packet.serialize();

    Tins::EthernetII packet =
        Tins::EthernetII(destination_mac, source_mac) /
        Tins::IPv6(destination_address, source_address) /
        Tins::ICMPv6(Tins::ICMPv6::Types::DEST_UNREACHABLE);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(hoplimit);
    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
    inner_icmpv6.code(4);
    inner_icmpv6.inner_pdu(Tins::RawPDU(serialized_embedded_packet));

    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const std::string expected_packet(serialized_packet.begin(),
                                      serialized_packet.end());

    /* Tests */
    try
    {
        reply.icmp_echo_reply(78, 1, {});
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require ICMP echo reply "
                  "information.");
    }

    std::ostringstream test_output;
    test_output << reply;
    EXPECT_EQ(
        test_output.str(),
        "REPLY ICMP_PORT_UNREACHABLE: fd00::1 (52:54:00:b2:fa:7f) -> fd00::2 "
        "(52:54:00:b2:fa:7e) Hoplimit=55: DPORT=34344 SPORT=46432 LENGTH=10");

    EXPECT_EQ(actual_packet, expected_packet);
    EXPECT_EQ(reply.get_type(), NodeReplyType::ICMP_PORT_UNREACHABLE);
}

TEST(NodeReplyTest, IcmpTimeExceededUdp)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7e");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("fd00::2");
    const Tins::IPv6Address original_destination_address("fd00::3");
    constexpr int hoplimit = 55;
    constexpr int udp_sport = 46432;
    constexpr int udp_dport = 34344;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    /* Reply */
    NodeReply reply(NodeReplyType::ICMP_TIME_EXCEEDED_UDP,
                    destination_mac,
                    destination_address,
                    source_mac,
                    source_address);
    reply.set_hoplimit(hoplimit);
    reply.udp_response(payload, udp_dport, udp_sport);
    reply.packet_reassembly(original_destination_address);
    const std::string actual_packet = reply.to_packet();

    /* Expected packet */
    Tins::IPv6 embedded_packet =
        Tins::IPv6(original_destination_address, destination_address) /
        Tins::UDP(udp_dport, udp_sport);
    embedded_packet.hop_limit(1);
    Tins::UDP& embedded_inner_udp = embedded_packet.rfind_pdu<Tins::UDP>();
    embedded_inner_udp.inner_pdu(Tins::RawPDU(payload));
    Tins::PDU::serialization_type serialized_embedded_packet =
        embedded_packet.serialize();

    Tins::EthernetII packet = Tins::EthernetII(destination_mac, source_mac) /
                              Tins::IPv6(destination_address, source_address) /
                              Tins::ICMPv6(Tins::ICMPv6::Types::TIME_EXCEEDED);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(hoplimit);
    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
    inner_icmpv6.inner_pdu(Tins::RawPDU(serialized_embedded_packet));

    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const std::string expected_packet(serialized_packet.begin(),
                                      serialized_packet.end());

    /* Tests */
    try
    {
        reply.icmp_echo_reply(78, 1, {});
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require ICMP echo reply "
                  "information.");
    }

    std::ostringstream test_output;
    test_output << reply;
    EXPECT_EQ(test_output.str(),
              "REPLY ICMP_TIME_EXCEEDED_UDP: fd00::1 (52:54:00:b2:fa:7f) -> "
              "fd00::2 (52:54:00:b2:fa:7e) Hoplimit=55 LENGTH=10");

    EXPECT_EQ(actual_packet, expected_packet);
    EXPECT_EQ(reply.get_type(), NodeReplyType::ICMP_TIME_EXCEEDED_UDP);
}

TEST(NodeReplyTest, IcmpNdp)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7e");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("fd00::2");

    /* Reply */
    NodeReply reply(NodeReplyType::ICMP_NDP,
                    destination_mac,
                    destination_address,
                    source_mac,
                    source_address);
    const std::string actual_packet = reply.to_packet();

    /* Expected packet */
    Tins::EthernetII packet =
        Tins::EthernetII(destination_mac, source_mac) /
        Tins::IPv6(destination_address, source_address) /
        Tins::ICMPv6(Tins::ICMPv6::Types::NEIGHBOUR_ADVERT);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(255);
    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
    inner_icmpv6.target_addr(source_address);
    inner_icmpv6.solicited(Tins::small_uint<1>(1));
    inner_icmpv6.router(Tins::small_uint<1>(1));
    inner_icmpv6.override(Tins::small_uint<1>(1));
    const Tins::ICMPv6::option address_option(
        Tins::ICMPv6::OptionTypes::TARGET_ADDRESS,
        source_mac.size(),
        &(*source_mac.begin()));
    inner_icmpv6.add_option(address_option);

    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const std::string expected_packet(serialized_packet.begin(),
                                      serialized_packet.end());

    /* Tests */
    try
    {
        reply.set_hoplimit(120);
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "ICMP NDP responses always have a hop limit of 255.");
    }

    try
    {
        reply.icmp_echo_reply(3, 5, {});
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require ICMP echo reply "
                  "information.");
    }

    try
    {
        reply.udp_response({}, 33434, 45834);
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require UDP information.");
    }

    try
    {
        reply.packet_reassembly(Tins::IPv6Address("fd00::"));
    }
    catch (const std::exception& e)
    {
        SUCCEED();
        EXPECT_EQ(std::string(e.what()),
                  "NodeReply has no type that would require original "
                  "destionation address information.");
    }

    std::ostringstream test_output;
    test_output << reply;
    EXPECT_EQ(test_output.str(),
              "REPLY ICMP_NDP: fd00::1 (52:54:00:b2:fa:7f) -> fd00::2 (52:54:00:b2:fa:7e)");

    EXPECT_EQ(actual_packet, expected_packet);
    EXPECT_EQ(reply.get_type(), NodeReplyType::ICMP_NDP);
}
