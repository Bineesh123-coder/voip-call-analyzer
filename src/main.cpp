#include "pcap_reader.h"


int main()
{
    PcapReader reader;
    reader.read_pcap("pcaps/SIP-Call-with-Proxy-Server.pcap");    
    return 0;
}



