#ifndef NODEREPLY_HPP
#define NODEREPLY_HPP

#include <tins/tins.h>

enum class NodeReplyType { ICMP_ECHO_REPLY, ICMP_TIME_EXCEEDED, ICMP_PORT_UNREACHABLE, ICMP_NDP };

class NodeReply
{
    public:
        NodeReply(NodeReplyType type, int icmp_identifier, int icmp_sequence);
        NodeReply(NodeReplyType type, Tins::RawPDU::payload_type udp_content, int udp_dport, int udp_sport);
        NodeReply(NodeReplyType type, Tins::HWAddress<6> source_mac);
        std::string to_packet();

        friend std::ostream& operator<<(std::ostream& os, const NodeReply & nodereply);

    private:
        NodeReplyType _type;

        /* ICMP ECHO REPLY */
        int _icmp_identifier;
        int _icmp_sequence;

        /* ICMP_TIME_EXCEEDED + ICMP_PORT_UNREACHABLE */
        Tins::RawPDU::payload_type /* aka std::vector<uint8_t> */ _udp_content;
        int _udp_dport;
        int _udp_sport;

        /* ICMP_NDP */
        Tins::HWAddress<6> _source_mac;
};

#endif
