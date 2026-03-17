# VoIP Call Analyzer

**A lightweight C++ tool to analyze VoIP calls from PCAP files or live network interfaces.**

Extracts SIP signaling (INVITE, BYE, From/To/Call-ID) and RTP media statistics (packet count, loss, jitter, duration, bitrate, MOS score estimate) for G.711 and other common codecs.

## Features

- Parse SIP messages (INVITE / 200 OK / BYE) to identify call sessions
- Extract caller/callee numbers, Call-ID, SDP (media port & codec)
- Track RTP streams (payload type, sequence numbers, timestamps)
- Calculate:
  - Packet loss
  - Inter-arrival jitter (RFC 3550 style approximation)
  - Call duration (from SIP or RTP timestamps)
  - Approximate bitrate
  - MOS score & subjective quality rating (Excellent → Poor)
- Supports offline PCAP analysis (`-r file.pcap`) and live capture (`-i interface`)
- Hourly rotating logs: `logs/YYYY/MM/DD/HH_voip_analyzer.log`
- Thread-safe logging with background rotation thread

## Example Output

[21:34:55.995] ===== CALL SUMMARY =====
[21:34:55.995] Call-ID: 1-1966@10.0.2.20
[21:34:55.995] Caller : sipp
[21:34:55.995] Callee : test
[21:34:55.995] Codec : 127 PT 127
[21:34:55.995] SIP Packets : 3
[21:34:55.995] RTP Packets : 426
[21:34:55.995] Packet lose : 0
[21:34:55.995] Jitter : 18.750000 ms
[21:34:55.995] Duration : 9 seconds
[21:34:55.995] --------------------------



## Requirements

- Linux (tested on Ubuntu/Mint/Debian)
- **libpcap** (`sudo apt install libpcap-dev`)
- C++17 compiler (g++)

# From project root
mkdir -p build && cd build
make clean make 

# OR direct compile (no CMake needed):
g++ -std=c++17 -Wall -Wextra -g \
    ../src/main.cpp ../src/pcap_reader.cpp ../src/sip_parser.cpp \
    ../src/call_session_manager.cpp ../src/logger.cpp \
    -I../include -o voip_analyzer -lpcap -pthread

./voip_analyzer -r pcaps/sip-rtp-g711.pcap
# or with sudo if needed for permissions on some filesystems
sudo ./voip_analyzer -r capture.pcap

sudo ./voip_analyzer -i wlo1          # Wi-Fi
# or
sudo ./voip_analyzer -i enp0s3        # Ethernet

Project Structure

voip-call-analyzer/
├── src/
│   ├── main.cpp
│   ├── pcap_reader.cpp
│   ├── sip_parser.cpp
│   ├── call_session_manager.cpp
│   └── logger.cpp
├── include/
│   ├── pcap_reader.h
│   ├── sip_parser.h
│   ├── call_session_manager.h
│   └── logger.h
├── pcaps/                  # sample captures (add your own)
├── logs/                   # auto-created: YYYY/MM/DD/HH_voip_analyzer.log
├── README.md
└── Makefile                


Limitations & Known Issues

Basic SDP parsing → only first payload type & port (no multi-codec / ICE / SRTP support)
Jitter & MOS are approximations (not full RTCP/XR compliant)
Only UDP-based SIP & RTP (no TCP/TLS yet)
No audio reconstruction / playback
RTP port mapping relies on SDP → calls without SDP in analyzed packets may miss media

Future Improvements (planned / ideas)

Support for SIP over TCP/TLS
Better codec detection (rtpmap parsing)
RTCP analysis (real jitter, packet loss reported by endpoints)
Call graph visualization (dot / mermaid)
JSON / CSV output for post-processing
MOS calculation using ITU-T E-model (with delay & codec factor)

Contributing
Feel free to open issues or pull requests!
Especially welcome:

Better SDP / From/To parsing (robust against weird formatting)
Support for more codecs (Opus, G.729, G.722…)
Tests with real captures

License
MIT License (or choose whatever fits your needs)
Feel free to use, modify, distribute.
Happy analyzing!