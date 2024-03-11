# Crazy traceroute

What happens if a traceroute with the same TTL/hop limit is received from two different source addresses? How will they react?

crazytrace is a network simulation program that can be used to see how different ping and traceroute implementations react when the sender address is different than expected.

## How it works?

crazytrace uses several libraries and tricks to create a virtual TAP adapter behind which the simulated network is hidden.

- libtuntap: This library is used to create and close the TAP device.
- libtins: This library is used for packet parsing and crafting.
- yaml-cpp: This library is used to read the configuration file in YAML format.
- Boost.log: This library is used as a logger. Various log levels can be set in the configuration file.
- Boost.Asio: This library is used to communicate with the socket of the TAP device and to receive and send several packets simultaneously (asynchronously).

Here is how the program works:
1. reading the configuration file
2. setting the log level
3. output of the libtuntap version
4. output of the configuration
5. creation of the TAP device
6. initialization of the network simulator
7. starting the network simulator

The following is how the network simulator works when a packet is received:
1. reading the packet with tins
2. reading the packet into a NodeRequest
3. generate a NodeReply using the configuration
4. check whether a reply should be sent
4a. If no, abort
5. if yes, create a NodeReply package using libtins
6. write the packet to the socket of the TAP device

## Configuration file

The following is a example configuration file:
```yaml
---
log_level: info
nodes:
  - mac: 02:B1:4F:7E:9D:C3
    addresses: [fe80::b1:4fff:fe7e:9dc3]
    hoplimit: 255
    nodes:
      - addresses: [fd00::1]
        nodes:
          - addresses: [fd00::21, fd00::22, fd00::23]
            nodes:
              - addresses: [fd00::3]
  - mac: 02:B1:4F:7E:9D:C4
    addresses: [fe80::b1:4fff:fe7e:9dc4]
```

This would generate the following topology:
![Topology](topology.png)

If a node has several IPv6 addresses, it listens to all of them and responds randomly with one.
If no hop limit is specified, 64 is used.

To reach the subnet `fd00::/64` under Linux, you can use the following command:
```
# ip route add fd00::/64 dev crazytrace via fe80::b1:4fff:fe7e:9dc3
```

### Which MAC addresses can I use without any problems?

The following belong to the locally administered range and can be used without any problems:
```
x2-xx-xx-xx-xx-xx
x6-xx-xx-xx-xx-xx
xA-xx-xx-xx-xx-xx
xE-xx-xx-xx-xx-xx
```

## How do I start crazytrace?

```
# /path/to/crazytrace /path/to/config.yaml
```
