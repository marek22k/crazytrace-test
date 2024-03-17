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
        explicit tun_tap(const std::string& ifname, tun_tap_mode mode);
        void set_ip(const std::string& ip, int netmask);
        void set_mtu(int mtu);
        void up();
        void down();
        [[nodiscard]] int native_handler();
    
    private:
        class TunTapDestroyer final {
            public:
                constexpr void operator()(device * dev) const noexcept {
                    if (dev != nullptr)
                        ::tuntap_destroy(dev);
                }
        };
        std::unique_ptr<device, TunTapDestroyer> _device;
};

#endif