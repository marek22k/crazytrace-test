# Crazy traceroute

What happens if a traceroute with the same TTL/hop limit is received from two different source addresses? How will they react?

## Traceroutes with several probes per hop

- traceroute
- hoptracker

Accordingly, only these would have to react "specifically", especially if there are responses from two different hops for several probes for one hop.

## Options for identifying a trace ( in case of sending multiple probes)

### UDP-based methods

| Trace method | Constant source address | Constant source port | Constant destination address | Constant destination port | Identifier |
| --- | --- | --- | --- | --- | --- |
| traceroute (default, udp) | Yes | No | Yes | No | src addr + dest addr |
| traceroute (default, udp, set sport) | Yes | Yes | Yes | No | src addr/port + dest addr |
| tracepath (default)  | Yes | Yes | Yes | No | src addr/port + dest addr |
| hoptracker | Yes | Yes | Yes | Yes | src addr/port + dest addr/port |
| scamper (default, udp-paris) | Yes | Yes | Yes | Yes | src addr/port + dest addr/port |
| scamper (udp) | Yes | Yes | Yes | No | src addr/port + dest addr |

Common identifier: src addr + dest addr
Second common identifier: src addr/port + dest addr

### ICMP-based methods

| Trace method | Constant source address | Constant destination address | Constant ICMP identifier | Constant ICMP sequence | Identifier |
| traceroute (icmp) | Yes | Yes | Yes | No | src addr + dest addr + icmp identifier |
| scamper (icmp, icmp-paris) | Yes | Yes | Yes | No | src addr + dest addr + icmp identifier |

Common identifier: src addr + dest addr + icmp identifier
