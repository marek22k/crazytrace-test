#include "nodereply.hpp"

NodeReply::NodeReply(NodeReplyType type, int icmp_identifier, int icmp_sequence) :
    _type(type), _icmp_identifier(icmp_identifier), _icmp_sequence(icmp_sequence)
{}

NodeReply::NodeReply(NodeReplyType type, Tins::RawPDU::payload_type udp_content, int udp_dport, int udp_sport) :
    _type(type), _udp_content(udp_content), _udp_dport(udp_dport), _udp_sport(udp_sport)
{}

NodeReply::NodeReply(NodeReplyType type, Tins::HWAddress<6> source_mac) :
    _type(type), _source_mac(source_mac)
{}

std::ostream& operator<<(std::ostream& os, NodeRequest const & noderequest)
{
    std::string type_string;
    switch (noderequest._type)
    {
        case NodeRequestType::UNKNOWN:
            type_string = "UNKNOWN";
            break;
        case NodeRequestType::ICMP_ECHO_REQUEST:
            type_string = "ICMP_ECHO_REQUEST";
            break;
        case NodeRequestType::ICMP_NDP:
            type_string = "ICMP_NDP";
            break;
        case NodeRequestType::UDP:
            type_string = "UDP";
            break;
    }
    os << type_string << ": " <<
          noderequest._source_address << " (" << noderequest._source_mac << ") -> " <<
          noderequest._destination_address << " (" << noderequest._destination_mac << ") " <<
          "Hoplimit=" << noderequest._hoplimit;

    switch (noderequest._type)
    {
        case NodeRequestType::ICMP_ECHO_REQUEST:
            os << ": ID=" << noderequest._icmp_identifier << " SEQ=" << noderequest._icmp_sequence;
            break;
        case NodeRequestType::ICMP_NDP:
            os << ": Looking for " << noderequest._destination_address;
            break;
        case NodeRequestType::UDP:
            
            break;
        default:
            break;
    }
    return os;
}
