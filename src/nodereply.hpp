#ifndef NODEREPLY_HPP
#define NODEREPLY_HPP

#include <string>
#include <tins/tins.h>

enum class NodeReplyType { NOREPLY, ICMP_ECHO_REPLY, ICMP_TIME_EXCEEDED, ICMP_PORT_UNREACHABLE, ICMP_NDP };

class NodeReply
{
    public:
        NodeReply(NodeReplyType type);
        NodeReply(
            NodeReplyType type,
            Tins::HWAddress<6> destination_mac, Tins::IPv6Address destination_address,
            Tins::HWAddress<6> source_mac, Tins::IPv6Address source_address
        );

        /* TODO: Set hoplimit */
        void icmp_echo_reply(int icmp_identifier, int icmp_sequence);
        void udp_response(Tins::RawPDU::payload_type udp_content, int udp_dport, int udp_sport);

        std::string to_packet();
        NodeReplyType get_type();

        friend std::ostream& operator<<(std::ostream& os, const NodeReply & nodereply);

    private:
        NodeReplyType _type;
        int _hoplimit;
        Tins::HWAddress<6> _source_mac;
        Tins::HWAddress<6> _destination_mac;
        Tins::IPv6Address _source_address; /* Also used for ICMP_NDP */
        Tins::IPv6Address _destination_address;

        /* ICMP ECHO REPLY */
        int _icmp_identifier;
        int _icmp_sequence;

        /* ICMP_TIME_EXCEEDED + ICMP_PORT_UNREACHABLE */
        Tins::RawPDU::payload_type /* aka std::vector<uint8_t> */ _udp_content;
        int _udp_dport;
        int _udp_sport;
};

#endif
