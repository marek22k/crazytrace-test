#ifndef NODEREPLY_HPP
#define NODEREPLY_HPP

#include <ostream>
#include <string>
#include <stdexcept>
#include <tins/tins.h>

enum class NodeReplyType {
    NOREPLY, /* No reply is to be sent. */
    ICMP_ECHO_REPLY, /* An ICMP ECHO REPLY packet is sent in response. */
    ICMP_TIME_EXCEEDED_ICMP_ECHO_REQUEST, /* An ICMP TIME EXCEEDED packet is sent in response to an ICMP ECHO_REQUEST packet. */
    ICMP_PORT_UNREACHABLE, /* An ICMP PORT UNREACHABLE packet is sent in response. */
    ICMP_TIME_EXCEEDED_UDP, /* An ICMP TIME EXCEEDED packet is sent in response to an UDP packet. */
    ICMP_NDP /* A neighbor advertisement is sent. */
};

class NodeReply
{
    public:
        NodeReply(NodeReplyType type);
        NodeReply(
            NodeReplyType type,
            Tins::HWAddress<6> destination_mac, Tins::IPv6Address destination_address,
            Tins::HWAddress<6> source_mac, Tins::IPv6Address source_address
        );

        void set_hoplimit(int hoplimit);
        void icmp_echo_reply(int icmp_identifier, int icmp_sequence, Tins::RawPDU::payload_type payload);
        void packet_reassembly(Tins::IPv6Address original_destination_address);
        void udp_response(Tins::RawPDU::payload_type payload, int udp_dport, int udp_sport);

        std::string to_packet();
        NodeReplyType get_type();

        friend std::ostream& operator<<(std::ostream& os, const NodeReply & nodereply);

    private:
        NodeReplyType _type;
        int _hoplimit;
        Tins::HWAddress<6> _destination_mac;
        Tins::IPv6Address _destination_address;
        Tins::HWAddress<6> _source_mac;
        Tins::IPv6Address _source_address; /* Also used for ICMP_NDP */

        /* ICMP ECHO REPLY */
        int _icmp_identifier;
        int _icmp_sequence;

        /* ICMP_PORT_UNREACHABLE */
        Tins::RawPDU::payload_type /* aka std::vector<uint8_t> */ _payload;
        int _udp_dport;
        int _udp_sport;

        /* ICMP TIME EXCEEDED */
        Tins::IPv6Address _original_destination_address; 
};

#endif
