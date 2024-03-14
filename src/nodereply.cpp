#include "nodereply.hpp"

NodeReply::NodeReply(NodeReplyType type) :
    _type(type)
{}

NodeReply::NodeReply(
    NodeReplyType type,
    Tins::HWAddress<6> destination_mac, Tins::IPv6Address destination_address,
    Tins::HWAddress<6> source_mac, Tins::IPv6Address source_address
) :
    _type(type),
    _destination_mac(destination_mac), _destination_address(destination_address),
    _source_mac(source_mac), _source_address(source_address)
{}

void NodeReply::set_hoplimit(int hoplimit) noexcept
{
    this->_hoplimit = hoplimit;
}

void NodeReply::icmp_echo_reply(int icmp_identifier, int icmp_sequence, const Tins::RawPDU::payload_type& payload) noexcept
{
    this->_icmp_identifier = icmp_identifier;
    this->_icmp_sequence = icmp_sequence;
    this->_payload = payload;
}

void NodeReply::packet_reassembly(Tins::IPv6Address original_destination_address) noexcept
{
    this->_original_destination_address = original_destination_address;
}

void NodeReply::udp_response(const Tins::RawPDU::payload_type& payload, int udp_dport, int udp_sport) noexcept
{
    this->_payload = payload;
    this->_udp_dport = udp_dport;
    this->_udp_sport = udp_sport;
}

std::string NodeReply::to_packet() const
{
    switch(this->_type)
    {
        case NodeReplyType::ICMP_ECHO_REPLY:
        {
            Tins::EthernetII packet = Tins::EthernetII(this->_destination_mac, this->_source_mac) /
                                      Tins::IPv6(this->_destination_address, this->_source_address) /
                                      Tins::ICMPv6(Tins::ICMPv6::Types::ECHO_REPLY);
            Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
            inner_ipv6.hop_limit(this->_hoplimit);
            Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
            inner_icmpv6.identifier(this->_icmp_identifier);
            inner_icmpv6.sequence(this->_icmp_sequence);
            inner_icmpv6.inner_pdu(Tins::RawPDU(this->_payload));

            Tins::PDU::serialization_type serialized_packet = packet.serialize();
            std::string raw_packet(serialized_packet.begin(), serialized_packet.end());
            return raw_packet;
        }
        case NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST:
        case NodeReplyType::ICMP_TIME_EXCEEDED_UDP:
        case NodeReplyType::ICMP_PORT_UNREACHABLE:
        {
            /* Recreation of the receiving packet */
            Tins::IPv6 receiving_ipv6 = Tins::IPv6(this->_original_destination_address, this->_destination_address);
            receiving_ipv6.hop_limit(1);
            switch (this->_type)
            {
                case NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST:
                {
                    Tins::ICMPv6 receiving_icmpv6 = Tins::ICMPv6(Tins::ICMPv6::Types::ECHO_REQUEST);
                    receiving_icmpv6.identifier(this->_icmp_identifier);
                    receiving_icmpv6.sequence(this->_icmp_sequence);
                    receiving_icmpv6.inner_pdu(Tins::RawPDU(this->_payload));
                    receiving_ipv6.inner_pdu(receiving_icmpv6);
                    break;
                }
                case NodeReplyType::ICMP_TIME_EXCEEDED_UDP:
                case NodeReplyType::ICMP_PORT_UNREACHABLE:
                {
                    Tins::UDP receiving_udp = Tins::UDP(this->_udp_dport, this->_udp_sport);
                    receiving_udp.inner_pdu(Tins::RawPDU(this->_payload));
                    receiving_ipv6.inner_pdu(receiving_udp);
                    break;
                }
                default:
                    /* Fatal error */
                    break;
            }
            Tins::PDU::serialization_type serialized_receiving_packet = receiving_ipv6.serialize();
            serialized_receiving_packet.resize(1000);

            switch (this->_type)
            {
                case NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST:
                case NodeReplyType::ICMP_TIME_EXCEEDED_UDP:
                {
                    Tins::EthernetII packet = Tins::EthernetII(this->_destination_mac, this->_source_mac) /
                                              Tins::IPv6(this->_destination_address, this->_source_address) /
                                              Tins::ICMPv6(Tins::ICMPv6::Types::TIME_EXCEEDED);
                    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
                    inner_ipv6.hop_limit(this->_hoplimit);
                    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
                    inner_icmpv6.inner_pdu(Tins::RawPDU(serialized_receiving_packet));

                    Tins::PDU::serialization_type serialized_packet = packet.serialize();
                    std::string raw_packet(serialized_packet.begin(), serialized_packet.end());
                    return raw_packet;
                }
                case NodeReplyType::ICMP_PORT_UNREACHABLE:
                {
                    Tins::EthernetII packet = Tins::EthernetII(this->_destination_mac, this->_source_mac) /
                                              Tins::IPv6(this->_destination_address, this->_source_address) /
                                              Tins::ICMPv6(Tins::ICMPv6::Types::DEST_UNREACHABLE);
                    Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
                    inner_ipv6.hop_limit(this->_hoplimit);
                    Tins::ICMPv6& inner_icmpv6 = inner_ipv6.rfind_pdu<Tins::ICMPv6>();
                    inner_icmpv6.code(4);
                    inner_icmpv6.inner_pdu(Tins::RawPDU(serialized_receiving_packet));

                    Tins::PDU::serialization_type serialized_packet = packet.serialize();
                    std::string raw_packet(serialized_packet.begin(), serialized_packet.end());
                    return raw_packet;
                }
                default: [[unlikely]]
                    throw std::runtime_error("Attempt to create a packet, but the packet type has suddenly changed. As a result, no response could be generated.");
            }
        }
        case NodeReplyType::ICMP_NDP:
        {
            Tins::EthernetII packet = Tins::EthernetII(this->_destination_mac, this->_source_mac) /
                                   Tins::IPv6(this->_destination_address, this->_source_address) /
                                   Tins::ICMPv6(Tins::ICMPv6::Types::NEIGHBOUR_ADVERT);
            Tins::IPv6& inner_ipv6 = packet.rfind_pdu<Tins::IPv6>();
            inner_ipv6.hop_limit(255);
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
        }
        default: [[unlikely]]
            throw std::runtime_error("Attempt to create a packet, although there is no reply.");
    }

    [[unlikely]]
    throw std::runtime_error("Attempt to create a packet, although there is no reply.");
}


NodeReplyType NodeReply::get_type() const noexcept
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
        case NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST:
            type_string = "ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST";
            break;
        case NodeReplyType::ICMP_PORT_UNREACHABLE:
            type_string = "ICMP_PORT_UNREACHABLE";
            break;
        case NodeReplyType::ICMP_TIME_EXCEEDED_UDP:
            type_string = "ICMP_TIME_EXCEEDED_UDP";
            break;
        case NodeReplyType::ICMP_NDP:
            type_string = "ICMP_NDP";
            break;
        default:
            type_string = "UNKNOWN";
            break;
    }
    os << "REPLY " << type_string << ": " <<
          nodereply._source_address << " (" << nodereply._source_mac << ") -> " <<
          nodereply._destination_address << " (" << nodereply._destination_mac << ")";

    switch (nodereply._type)
    {
        case NodeReplyType::ICMP_ECHO_REPLY:
        {
            os << " Hoplimit=" << nodereply._hoplimit <<
                  ": ID=" << nodereply._icmp_identifier <<
                  " SEQ=" << nodereply._icmp_sequence <<
                  " Payload:" << std::hex << std::setw(2);
                  for (auto byte = nodereply._payload.begin(); byte != nodereply._payload.end(); byte++)
                  {
                    os << " " << static_cast<int>(*byte);
                  }
            break;
        }
        case NodeReplyType::ICMP_PORT_UNREACHABLE:
            os << " Hoplimit=" << nodereply._hoplimit <<
                  ": DPORT=" << nodereply._udp_dport <<
                  " SPORT=" << nodereply._udp_sport <<
                  " LENGTH=" << nodereply._payload.size();
            break;
        case NodeReplyType::ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST:
        case NodeReplyType::ICMP_TIME_EXCEEDED_UDP:
            os << " Hoplimit=" << nodereply._hoplimit <<
                  " LENGTH=" << nodereply._payload.size();
            break;
        case NodeReplyType::ICMP_NDP:
            os << ": NDP";
            break;
        default:
            break;
    }
    return os;
}
