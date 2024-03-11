#ifndef NODEREQUEST_HPP
#define NODEREQUEST_HPP

#include <ostream>
#include <iomanip>
#include <tins/tins.h>
#include <tins/constants.h>

enum class NodeRequestType { UNKNOWN, ICMP_ECHO_REQUEST, ICMP_NDP, UDP };

class NodeRequest
{
    public:
        NodeRequest(Tins::EthernetII packet);
        NodeRequestType get_type();
        const Tins::HWAddress<6>& get_source_mac();
        const Tins::HWAddress<6>& get_destination_mac();
        const Tins::IPv6Address& get_source_address();
        const Tins::IPv6Address& get_destination_address();
        int get_hoplimit();

        int get_udp_sport();
        int get_udp_dport();

        int get_icmp_identifier();
        int get_icmp_sequence();

        const Tins::RawPDU::payload_type& get_payload();

        friend std::ostream& operator<<(std::ostream& os, const NodeRequest & noderequest);

    private:
        NodeRequestType _type;
        Tins::HWAddress<6> _source_mac;
        Tins::HWAddress<6> _destination_mac;
        Tins::IPv6Address _source_address;
        Tins::IPv6Address _destination_address;
        int _hoplimit;

        int _udp_dport;
        int _udp_sport;

        int _icmp_identifier;
        int _icmp_sequence;

        /* Used for UDP and ICMP */
        Tins::RawPDU::payload_type /* aka std::vector<uint8_t> */ _payload;
};

#endif
