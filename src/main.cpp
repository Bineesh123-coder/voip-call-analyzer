#include "pcap_reader.h"


int main()
{
    PcapReader reader;
    // reader.read_pcap("pcaps/sip-direct.pcapng"); 
    // reader.read_pcap("pcaps/SIP-Call-with-Proxy-Server.pcap");    
    // reader.read_pcap("pcaps/SIP-SDP-Example.pcap");
    // reader.read_pcap("pcaps/ciscotest.pcap");
    reader.read_pcap("pcaps/sip-rtp-g711.pcap");  
             
    return 0;
}



