#include <gtest/gtest.h>
#include <tins/tins.h>
#include "noderequest.hpp"

TEST(NodeRequestTest, EchoRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7e");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("fd00::2");
    constexpr int hoplimit = 55;
    constexpr int icmp_identifier = 56;
    constexpr int icmp_sequence = 1;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    Tins::EthernetII packet = Tins::EthernetII(destination_mac, source_mac) /
                              Tins::IPv6(destination_address, source_address) /
                              Tins::ICMPv6(Tins::ICMPv6::Types::ECHO_REQUEST);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(hoplimit);
    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
    inner_icmpv6.identifier(icmp_identifier);
    inner_icmpv6.sequence(icmp_sequence);
    inner_icmpv6.inner_pdu(Tins::RawPDU(payload));

    /* Serializing the package is necessary to automatically calculate checksums
     * and other fields. */
    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const Tins::EthernetII final_packet(serialized_packet.data(),
                                        serialized_packet.size());

    const NodeRequest request(final_packet);

    EXPECT_EQ(request.get_type(), NodeRequestType::ICMP_ECHO_REQUEST);
    EXPECT_EQ(request.get_source_mac(), source_mac);
    EXPECT_EQ(request.get_destination_mac(), destination_mac);
    EXPECT_EQ(request.get_source_address(), source_address);
    EXPECT_EQ(request.get_destination_address(), destination_address);
    EXPECT_EQ(request.get_hoplimit(), hoplimit);
    EXPECT_EQ(request.get_udp_sport(), 0);
    EXPECT_EQ(request.get_udp_dport(), 0);
    EXPECT_EQ(request.get_icmp_identifier(), icmp_identifier);
    EXPECT_EQ(request.get_icmp_sequence(), icmp_sequence);
    EXPECT_EQ(request.get_payload(), payload);

    std::ostringstream test_output;
    test_output << request;
    EXPECT_EQ(test_output.str(),
              "REQUEST ICMP_ECHO_REQUEST: fd00::1 (52:54:00:b2:fa:7f) -> "
              "fd00::2 (52:54:00:b2:fa:7e) Hoplimit=55: ID=56 SEQ=1 Payload: "
              "08 04 05 09 ff 00 00 00 00 00");
}

TEST(NodeRequestTest, NdpRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("33:33:ff:48:b2:ae");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("ff02::1:ff48:b2ae");
    const Tins::IPv6Address target_address("fd00::2");
    constexpr int hoplimit = 2;

    Tins::EthernetII packet =
        Tins::EthernetII(destination_mac, source_mac) /
        Tins::IPv6(destination_address, source_address) /
        Tins::ICMPv6(Tins::ICMPv6::Types::NEIGHBOUR_SOLICIT);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(hoplimit);
    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
    inner_icmpv6.target_addr(target_address);

    /* Serializing the package is necessary to automatically calculate checksums
     * and other fields. */
    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const Tins::EthernetII final_packet(serialized_packet.data(),
                                        serialized_packet.size());

    const NodeRequest request(final_packet);

    EXPECT_EQ(request.get_type(), NodeRequestType::ICMP_NDP);
    EXPECT_EQ(request.get_source_mac(), source_mac);
    EXPECT_EQ(request.get_destination_mac(), destination_mac);
    EXPECT_EQ(request.get_source_address(), source_address);
    EXPECT_EQ(request.get_destination_address(), target_address);
    EXPECT_EQ(request.get_hoplimit(), hoplimit);
    EXPECT_EQ(request.get_udp_sport(), 0);
    EXPECT_EQ(request.get_udp_dport(), 0);
    EXPECT_EQ(request.get_icmp_identifier(), 0);
    EXPECT_EQ(request.get_icmp_sequence(), 0);
    EXPECT_TRUE(request.get_payload().empty());

    std::ostringstream test_output;
    test_output << request;
    EXPECT_EQ(test_output.str(),
              "REQUEST ICMP_NDP: fd00::1 (52:54:00:b2:fa:7f) -> fd00::2 "
              "(33:33:ff:48:b2:ae) Hoplimit=2: Looking for fd00::2");
}

TEST(NodeRequestTest, UdpRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7e");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("fd00::2");
    constexpr int hoplimit = 55;
    constexpr int udp_sport = 54739;
    constexpr int udp_dport = 33434;
    const Tins::RawPDU::payload_type payload = {8, 4, 5, 9, 255, 0, 0, 0, 0, 0};

    Tins::EthernetII packet = Tins::EthernetII(destination_mac, source_mac) /
                              Tins::IPv6(destination_address, source_address) /
                              Tins::UDP(udp_dport, udp_sport);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(hoplimit);
    Tins::UDP& inner_udp = inner_ipv6.rfind_pdu<Tins::UDP>();
    inner_udp.inner_pdu(Tins::RawPDU(payload));

    /* Serializing the package is necessary to automatically calculate checksums
     * and other fields. */
    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const Tins::EthernetII final_packet(serialized_packet.data(),
                                        serialized_packet.size());

    const NodeRequest request(final_packet);

    EXPECT_EQ(request.get_type(), NodeRequestType::UDP);
    EXPECT_EQ(request.get_source_mac(), source_mac);
    EXPECT_EQ(request.get_destination_mac(), destination_mac);
    EXPECT_EQ(request.get_source_address(), source_address);
    EXPECT_EQ(request.get_destination_address(), destination_address);
    EXPECT_EQ(request.get_hoplimit(), hoplimit);
    EXPECT_EQ(request.get_udp_sport(), udp_sport);
    EXPECT_EQ(request.get_udp_dport(), udp_dport);
    EXPECT_EQ(request.get_icmp_identifier(), 0);
    EXPECT_EQ(request.get_icmp_sequence(), 0);
    EXPECT_EQ(request.get_payload(), payload);

    std::ostringstream test_output;
    test_output << request;
    EXPECT_EQ(
        test_output.str(),
        "REQUEST UDP: fd00::1 (52:54:00:b2:fa:7f) -> fd00::2 "
        "(52:54:00:b2:fa:7e) Hoplimit=55: DPORT=33434 SPORT=54739 LENGTH=10");
}

TEST(NodeRequestTest, UnknownRequest)
{
    const Tins::HWAddress<6> source_mac("52:54:00:b2:fa:7f");
    const Tins::HWAddress<6> destination_mac("52:54:00:b2:fa:7e");
    const Tins::IPv6Address source_address("fd00::1");
    const Tins::IPv6Address destination_address("fd00::2");
    constexpr int hoplimit = 2;

    Tins::EthernetII packet =
        Tins::EthernetII(destination_mac, source_mac) /
        Tins::IPv6(destination_address, source_address) /
        Tins::ICMPv6(Tins::ICMPv6::Types::NEIGHBOUR_ADVERT);
    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
    inner_ipv6.hop_limit(hoplimit);

    /* Serializing the package is necessary to automatically calculate checksums
     * and other fields. */
    const Tins::PDU::serialization_type serialized_packet = packet.serialize();
    const Tins::EthernetII final_packet(serialized_packet.data(),
                                        serialized_packet.size());

    const NodeRequest request(final_packet);

    EXPECT_EQ(request.get_type(), NodeRequestType::UNKNOWN);
    EXPECT_EQ(request.get_source_mac(), source_mac);
    EXPECT_EQ(request.get_destination_mac(), destination_mac);
    EXPECT_EQ(request.get_source_address(), source_address);
    EXPECT_EQ(request.get_destination_address(), destination_address);
    EXPECT_EQ(request.get_hoplimit(), hoplimit);
    EXPECT_EQ(request.get_udp_sport(), 0);
    EXPECT_EQ(request.get_udp_dport(), 0);
    EXPECT_EQ(request.get_icmp_identifier(), 0);
    EXPECT_EQ(request.get_icmp_sequence(), 0);
    EXPECT_TRUE(request.get_payload().empty());

    std::ostringstream test_output;
    test_output << request;
    EXPECT_EQ(test_output.str(),
              "REQUEST UNKNOWN: fd00::1 (52:54:00:b2:fa:7f) -> fd00::2 "
              "(52:54:00:b2:fa:7e) Hoplimit=2");
}
