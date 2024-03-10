#ifndef NODEREQUEST_HPP
#define NODEREQUEST_HPP

#include <ostream>
#include <tins/tins.h>
#include <tins/constants.h>

#include "nodecontainer.hpp"

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

        const Tins::RawPDU::payload_type& get_udp_content();
        int get_udp_sport();
        int get_udp_dport();

        int get_icmp_identifier();
        int get_icmp_sequence();

        friend std::ostream& operator<<(std::ostream& os, const NodeRequest & noderequest);

    private:
        NodeRequestType _type;
        Tins::HWAddress<6> _source_mac;
        Tins::HWAddress<6> _destination_mac;
        Tins::IPv6Address _source_address;
        Tins::IPv6Address _destination_address;
        int _hoplimit;

        Tins::RawPDU::payload_type /* aka std::vector<uint8_t> */ _udp_content;
        int _udp_dport;
        int _udp_sport;

        int _icmp_identifier;
        int _icmp_sequence;
};

#endif
