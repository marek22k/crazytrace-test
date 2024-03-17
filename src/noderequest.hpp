#ifndef NODEREQUEST_HPP
#define NODEREQUEST_HPP

#include <iomanip>
#include <ostream>
#include <tins/constants.h>
#include <tins/tins.h>

enum class NodeRequestType
{
    UNKNOWN,
    ICMP_ECHO_REQUEST,
    ICMP_NDP,
    UDP
};

class NodeRequest
{
    public:
        explicit NodeRequest(const Tins::EthernetII& packet);
        [[nodiscard]] NodeRequestType get_type() const noexcept;
        [[nodiscard]] const Tins::HWAddress<6>& get_source_mac() const noexcept;
        [[nodiscard]] const Tins::HWAddress<6>&
            get_destination_mac() const noexcept;
        [[nodiscard]] const Tins::IPv6Address&
            get_source_address() const noexcept;
        [[nodiscard]] const Tins::IPv6Address&
            get_destination_address() const noexcept;
        [[nodiscard]] int get_hoplimit() const noexcept;

        [[nodiscard]] int get_udp_sport() const noexcept;
        [[nodiscard]] int get_udp_dport() const noexcept;

        [[nodiscard]] int get_icmp_identifier() const noexcept;
        [[nodiscard]] int get_icmp_sequence() const noexcept;

        [[nodiscard]] const Tins::RawPDU::payload_type&
            get_payload() const noexcept;

        friend std::ostream& operator<<(std::ostream& os,
                                        const NodeRequest& noderequest);

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
