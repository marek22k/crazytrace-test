#include "tun_tap.hpp"

tun_tap::tun_tap(std::string ifname, tun_tap_mode mode)
{
    switch(mode)
    {
        case tun_tap_mode::tun:
        case tun_tap_mode::tap:
            this->_device.reset(::tuntap_init());
            break;
        default:
            throw std::runtime_error("Unknown device mode.");
            break;
    }

    int tuntap_mode_number = -1;
    switch(mode)
    {
        case tun_tap_mode::tun:
            tuntap_mode_number = TUNTAP_MODE_TUNNEL;
            break;
        case tun_tap_mode::tap:
            tuntap_mode_number = TUNTAP_MODE_ETHERNET;
            break;
    }
    int status = ::tuntap_start(this->_device.get(), tuntap_mode_number, TUNTAP_ID_ANY);
    if (status)
        throw std::runtime_error("Failed to start tuntap device.");

    status = ::tuntap_set_ifname(this->_device.get(), ifname.c_str());
    if (status)
        throw std::runtime_error("Failed to set ifname for tuntap device.");
}

void tun_tap::up()
{
    int status = ::tuntap_up(this->_device.get());
    if (status)
        throw std::runtime_error("Failed to bring tuntap device up.");
}

void tun_tap::down()
{
    int status = ::tuntap_down(this->_device.get());
    if (status)
        throw std::runtime_error("Failed to bring tuntap device down.");
}

void tun_tap::set_mtu(int mtu)
{
    if (mtu < 0 || mtu > 65535)
        throw std::invalid_argument("Invalid mtu value.");

    int status = ::tuntap_set_mtu(this->_device.get(), mtu);
    if (status)
        throw std::runtime_error("Failed to set mtu for tuntap device.");
}

void tun_tap::set_ip(std::string ip, int netmask)
{
    if (netmask < 0 || netmask > 128)
        throw std::invalid_argument("Invalid netmask value.");

    int status = ::tuntap_set_ip(this->_device.get(), ip.c_str(), netmask);
    if (status)
        throw std::runtime_error("Failed to set ip address for tuntap device.");
}

int tun_tap::native_handler()
{
    return ::tuntap_get_fd(this->_device.get());
}
