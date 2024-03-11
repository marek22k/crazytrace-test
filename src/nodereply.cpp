#include "nodereply.hpp"

NodeReply::NodeReply(NodeReplyType type) :
    _type(type)
{}

NodeReply::NodeReply(
    NodeReplyType type, int hoplimit,
    Tins::HWAddress<6> destination_mac, Tins::IPv6Address destination_address,
    Tins::HWAddress<6> source_mac, Tins::IPv6Address source_address
) :
    _type(type), _hoplimit(hoplimit),
    _destination_mac(destination_mac), _destination_address(destination_address),
    _source_mac(source_mac), _source_address(source_address)
{}

void NodeReply::icmp_echo_reply(int icmp_identifier, int icmp_sequence)
{
    this->_icmp_identifier = icmp_identifier;
    this->_icmp_sequence = icmp_sequence;
}

void NodeReply::udp_response(Tins::RawPDU::payload_type udp_content, int udp_dport, int udp_sport)
{
    this->_udp_content = udp_content;
    this->_udp_dport = udp_dport;
    this->_udp_sport = udp_sport;
}

std::string NodeReply::to_packet()
{
    switch(this->_type)
    {
        case NodeReplyType::ICMP_ECHO_REPLY:

            break;
        case NodeReplyType::ICMP_TIME_EXCEEDED:

            break;
        case NodeReplyType::ICMP_PORT_UNREACHABLE:

            break;
        case NodeReplyType::ICMP_NDP:
            Tins::EthernetII packet = Tins::EthernetII(this->_destination_mac, this->_source_mac) /
                                   Tins::IPv6(this->_destination_address, this->_source_address) /
                                   Tins::ICMPv6(Tins::ICMPv6::Types::NEIGHBOUR_ADVERT);
            Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
            Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
            inner_icmpv6.target_addr(this->_source_address);
            inner_icmpv6.solicited(Tins::small_uint<1>(1));
            inner_icmpv6.router(Tins::small_uint<1>(1));
            inner_icmpv6.override(Tins::small_uint<1>(1));
            Tins::ICMPv6::option address_option(
                Tins::ICMPv6::OptionTypes::TARGET_ADDRESS,
                this->_source_mac.size(),
                &(*this->_source_mac.begin())
            );
            inner_icmpv6.add_option(address_option);
            Tins::PDU::serialization_type serialized_packet = packet.serialize();
            std::string raw_packet(serialized_packet.begin(), serialized_packet.end());
            return raw_packet;

            break;
    }

    return std::string("test");
}


NodeReplyType NodeReply::get_type()
{
    return this->_type;
}

std::ostream& operator<<(std::ostream& os, NodeReply const & nodereply)
{
    if (nodereply._type == NodeReplyType::NOREPLY)
    {
        os << "NOREPLY";
        return os;
    }

    std::string type_string;
    switch (nodereply._type)
    {
        case NodeReplyType::ICMP_ECHO_REPLY:
            type_string = "ICMP_ECHO_REPLY";
            break;
        case NodeReplyType::ICMP_TIME_EXCEEDED:
            type_string = "ICMP_TIME_EXCEEDED";
            break;
        case NodeReplyType::ICMP_PORT_UNREACHABLE:
            type_string = "ICMP_PORT_UNREACHABLE";
            break;
        case NodeReplyType::ICMP_NDP:
            type_string = "ICMP_NDP";
            break;
    }
    os << "REPLY " << type_string << ": " <<
          nodereply._source_address << " (" << nodereply._source_mac << ") -> " <<
          nodereply._destination_address << " (" << nodereply._destination_mac << ") " <<
          "Hoplimit=" << nodereply._hoplimit;

    switch (nodereply._type)
    {
        case NodeReplyType::ICMP_ECHO_REPLY:
            os << ": ID=" << nodereply._icmp_identifier << " SEQ=" << nodereply._icmp_sequence;
            break;
        case NodeReplyType::ICMP_TIME_EXCEEDED:
        case NodeReplyType::ICMP_PORT_UNREACHABLE:
            os << ": DPORT=" << nodereply._udp_dport <<
                  " SPORT=" << nodereply._udp_sport <<
                  " LENGTH=" << nodereply._udp_content.size();
            break;
        case NodeReplyType::ICMP_NDP:
            os << ": NDP";
            break;
    }
    return os;
}
