#include "pcap_reader.h"
#include "sip_parser.h"
#include "call_session_manager.h"

using namespace std;

CallSessionManager call_session;

void packet_handler(u_char*, const struct pcap_pkthdr* header, const u_char* packet)
{
    int ethernet_header = 14;

    const u_char* ip_header = packet + ethernet_header;
    int ip_header_len = (ip_header[0] & 0x0F) * 4;

    const u_char* udp_header = ip_header + ip_header_len;

    uint16_t src_port = (udp_header[0] << 8) | udp_header[1];
    uint16_t dst_port = (udp_header[2] << 8) | udp_header[3];

    // UDP payload start
    const u_char* payload = packet + 14 + ip_header_len + 8;
    int payload_len = header->len - (14 + ip_header_len + 8);
    
    // SIP detection
    if(src_port == 5060 || dst_port == 5060)
    {
        //std::cout << "Packet captured length: " << header->len << std::endl;
        std::string sip_data((char*)payload, payload_len);

        SIPMessage msg;
        SIPParser parser;

        if(parser.parse(sip_data, msg))
        {
            double timestamp =
            header->ts.tv_sec + header->ts.tv_usec / 1000000.0;

            call_session.process_sip(msg, timestamp);
        }
    }
    else{

        string call_id;

        if(call_session.rtp_port_map.count(src_port))
            call_id = call_session.rtp_port_map[src_port];

        if(call_session.rtp_port_map.count(dst_port))
            call_id = call_session.rtp_port_map[dst_port];

        if(!call_id.empty())
        {
            call_session.process_rtp(call_id,payload,payload_len,header->ts.tv_sec);
        }
    } 

}

void PcapReader::read_pcap(const std::string &file)
{   
    try{

        char errbuf[PCAP_ERRBUF_SIZE];

        pcap_t *handle = pcap_open_offline(file.c_str(), errbuf);

        if(!handle)
        {
            std::cout << "Error opening pcap file: " << errbuf << std::endl;
            return;
        }

        std::cout << "Reading packets from: " << file << std::endl;

        const char* filter_exp = "udp && (port 5060 or port 5061 or portrange 10000-65535)";

        struct bpf_program filter;

        if (pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1)
        {
            std::cout << "Filter compile error\n";
            return;
        }

        if (pcap_setfilter(handle, &filter) == -1)
        {
            std::cout << "Filter set error\n";
            return;
        }

        pcap_loop(handle, 0, packet_handler, nullptr);

        pcap_close(handle);

    
    }
    catch(const exception &e)
    {
       cout<<"ERROR:PcapReader::read_pcap" <<endl;
    }
    
}


void PcapReader::capture_live(const std::string& interface)
{
    char errbuf[PCAP_ERRBUF_SIZE];

    pcap_t* handle = pcap_open_live(
        interface.c_str(), 
        65535,   // max packet size
        1,       // promiscuous mode
        1000,    // timeout ms
        errbuf
    );

    if(!handle)
    {
        std::cout << "Error opening interface: " << errbuf << std::endl;
        return;
    }

    std::cout << "Listening on interface: " << interface << std::endl;

    const char* filter_exp = "udp && (port 5060 or port 5061 or portrange 10000-65535)";

    struct bpf_program filter;

    if(pcap_compile(handle, &filter, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        std::cout << "Filter compile error\n";
        return;
    }

    if(pcap_setfilter(handle, &filter) == -1)
    {
        std::cout << "Filter set error\n";
        return;
    }

    pcap_loop(handle, 0, packet_handler, nullptr);

    pcap_close(handle);
}