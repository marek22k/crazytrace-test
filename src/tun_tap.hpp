#ifndef TUNTAP_HPP
#define TUNTAP_HPP

#include <memory>
#include <stdexcept>
#include <string>

#include <tuntap.h>

enum class tun_tap_mode
{
    tun = 0,
    tap
};

class tun_tap
{
    public:
        tun_tap(std::string ifname, tun_tap_mode mode);
        void set_ip(std::string ip, int netmask);
        void set_mtu(int mtu);
        void up();
        void down();
        int native_handler();
    
    private:
        class TunTapDestroyer final {
            public:
                constexpr void operator()(device * dev) const noexcept {
                    if (dev)
                        ::tuntap_destroy(dev);
                }
        };
        std::unique_ptr<device, TunTapDestroyer> _device;
};

#endif