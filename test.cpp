#include <pcap.h>
#include <iostream>

void packet_handler(u_char*, const struct pcap_pkthdr* header, const u_char*)
{
    std::cout << "Packet captured: " << header->len << std::endl;
}

int main()
{
    char errbuf[PCAP_ERRBUF_SIZE];

    pcap_t *handle = pcap_open_live("wlo1", BUFSIZ, 1, 1000, errbuf);

    if(!handle)
    {
        std::cout << "Error opening device\n";
        return 1;
    }

    pcap_loop(handle, 0, packet_handler, nullptr);

    pcap_close(handle);
}