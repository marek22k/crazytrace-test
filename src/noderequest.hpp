#ifndef NODEREQUEST_HPP
#define NODEREQUEST_HPP

#include <ostream>
#include <tins/tins.h>

#include "nodecontainer.hpp"
#include "nodereply.hpp"

enum class NodeRequestType { ICMP, UDP, UNKNOWN };

class NodeRequest
{
    public:
        NodeRequest(Tins::EthernetII packet);
        NodeRequestType get_type();
        Tins::IPv6Address get_target_address();
        int get_hoplimit();

        int get_udp_content();
        int get_udp_sport();
        int get_udp_dport();

        int get_icmp_identifier();
        int get_icmp_sequence();

        NodeReply get_reply(NodeContainer root);

        friend std::ostream& operator<<(std::ostream& os, NodeRequest const & noderequest);

    private:
        NodeRequestType _type;
        Tins::IPv6Address _address;
        int _hoplimit;

        std::string _udp_content;
        int _udp_dport;
        int _udp_sport;

        int _icmp_identifier;
        int _icmp_sequence;
};

#endif
