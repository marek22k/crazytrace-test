#include "noderequest.hpp"

NodeRequest::NodeRequest(Tins::EthernetII packet)
{

}

NodeRequestType NodeRequest::get_type()
{

}

Tins::IPv6Address NodeRequest::get_target_address()
{

}

int NodeRequest::get_hoplimit()
{

}


int NodeRequest::get_udp_content()
{

}

int NodeRequest::get_udp_sport()
{

}

int NodeRequest::get_udp_dport()
{

}


int NodeRequest::get_icmp_identifier()
{

}

int NodeRequest::get_icmp_sequence()
{

}


NodeReply NodeRequest::get_reply(NodeContainer root)
{

}


std::ostream& NodeRequest::operator<<(std::ostream& os, NodeRequest const & noderequest)
{

}

