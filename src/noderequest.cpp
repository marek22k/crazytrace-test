#include "noderequest.hpp"
#include <iostream>

NodeRequest::NodeRequest(const Tins::EthernetII& packet) :
    _type(NodeRequestType::UNKNOWN),
    _hoplimit(0),
    _udp_dport(0),
    _udp_sport(0),
    _icmp_identifier(0),
    _icmp_sequence(0)
{
    this->_source_mac = packet.src_addr();
    this->_destination_mac = packet.dst_addr();

    if (packet.find_pdu<Tins::IPv6>() != nullptr)
    {
        const Tins::IPv6& ipv6 = packet.rfind_pdu<Tins::IPv6>();

        this->_source_address = ipv6.src_addr();
        this->_destination_address = ipv6.dst_addr();
        this->_hoplimit = static_cast<int>(ipv6.hop_limit());

        switch (ipv6.next_header())
        {
            case Tins::Constants::IP::PROTO_ICMPV6:
            {
                const Tins::ICMPv6& icmpv6 = ipv6.rfind_pdu<Tins::ICMPv6>();
                switch (icmpv6.type())
                {
                    case Tins::ICMPv6::Types::NEIGHBOUR_SOLICIT:
                        this->_type = NodeRequestType::ICMP_NDP;
                        if (!icmpv6.has_target_addr())
                            throw std::runtime_error(
                                "ICMP NDP packet has no target address "
                                "attribute.");

                        this->_destination_address = icmpv6.target_addr();
                        break;
                    case Tins::ICMPv6::Types::ECHO_REQUEST:
                    {
                        this->_type = NodeRequestType::ICMP_ECHO_REQUEST;
                        this->_icmp_identifier =
                            static_cast<int>(icmpv6.identifier());
                        this->_icmp_sequence =
                            static_cast<int>(icmpv6.sequence());
                        const Tins::RawPDU& raw_icmpv6 =
                            icmpv6.rfind_pdu<Tins::RawPDU>();
                        this->_payload = raw_icmpv6.payload();
                        break;
                    }
                    default:
                        /* Not supported */
                        break;
                }
                break;
            }
            case Tins::Constants::IP::PROTO_UDP:
            {
                const Tins::UDP& udp = packet.rfind_pdu<Tins::UDP>();
                const Tins::RawPDU& raw_udp = udp.rfind_pdu<Tins::RawPDU>();
                this->_type = NodeRequestType::UDP;
                this->_udp_dport = udp.dport();
                this->_udp_sport = udp.sport();
                this->_payload = raw_udp.payload();
                break;
            }
            default:
                /* Not supported */
                break;
        }
    }
}

NodeRequestType NodeRequest::get_type() const noexcept
{
    return this->_type;
}

const Tins::HWAddress<6>& NodeRequest::get_source_mac() const noexcept
{
    return this->_source_mac;
}

const Tins::HWAddress<6>& NodeRequest::get_destination_mac() const noexcept
{
    return this->_destination_mac;
}

const Tins::IPv6Address& NodeRequest::get_source_address() const noexcept
{
    return this->_source_address;
}

const Tins::IPv6Address& NodeRequest::get_destination_address() const noexcept
{
    return this->_destination_address;
}

int NodeRequest::get_hoplimit() const noexcept
{
    return this->_hoplimit;
}

int NodeRequest::get_udp_sport() const noexcept
{
    return this->_udp_sport;
}

int NodeRequest::get_udp_dport() const noexcept
{
    return this->_udp_dport;
}

int NodeRequest::get_icmp_identifier() const noexcept
{
    return this->_icmp_identifier;
}

int NodeRequest::get_icmp_sequence() const noexcept
{
    return this->_icmp_sequence;
}

const Tins::RawPDU::payload_type& NodeRequest::get_payload() const noexcept
{
    return this->_payload;
}

std::ostream& operator<<(std::ostream& os, NodeRequest const & noderequest)
{
    const auto os_flags = os.flags();

    std::string type_string;
    switch (noderequest._type)
    {
        case NodeRequestType::ICMP_ECHO_REQUEST:
            type_string = "ICMP_ECHO_REQUEST";
            break;
        case NodeRequestType::ICMP_NDP:
            type_string = "ICMP_NDP";
            break;
        case NodeRequestType::UDP:
            type_string = "UDP";
            break;
        default:
            type_string = "UNKNOWN";
            break;
    }
    os << "REQUEST " << type_string << ": " << noderequest._source_address
       << " (" << noderequest._source_mac << ") -> "
       << noderequest._destination_address << " ("
       << noderequest._destination_mac << ") "
       << "Hoplimit=" << noderequest._hoplimit;

    switch (noderequest._type)
    {
        case NodeRequestType::ICMP_ECHO_REQUEST:
        {
            os << ": ID=" << noderequest._icmp_identifier
               << " SEQ=" << noderequest._icmp_sequence
               << " Payload:" << std::hex;
            for (const auto& byte : noderequest._payload)
            {
                os << " " << std::setfill('0') << std::setw(2)
                   << static_cast<int>(byte);
            }
            break;
        }
        case NodeRequestType::ICMP_NDP:
            os << ": Looking for " << noderequest._destination_address;
            break;
        case NodeRequestType::UDP:
            os << ": DPORT=" << noderequest._udp_dport
               << " SPORT=" << noderequest._udp_sport
               << " LENGTH=" << noderequest._payload.size();
            break;
        default:
            break;
    }

    os.flags(os_flags);
    return os;
}
