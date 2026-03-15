#include "pcap_reader.h"
#include "sip_parser.h"
#include "call_session_manager.h"

using namespace std;

CallSessionManager call_session;

void packet_handler(u_char*, const struct pcap_pkthdr* header, const u_char* packet)
{
    std::cout << "Packet captured length: " << header->len << std::endl;

    std::string packet_data((char*)packet, header->len);

    SIPMessage msg;
    SIPParser parser;
    

    if(parser.parse(packet_data, msg))
    {
        std::cout << "------ SIP MESSAGE ------\n";
        std::cout << "Method : " << msg.method << std::endl;
        std::cout << "Caller : " << msg.caller << std::endl;
        std::cout << "Callee : " << msg.callee << std::endl;
        std::cout << "CallID : " << msg.call_id << std::endl;
        std::cout << "-------------------------\n";

        call_session.process_sip(msg);

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

        struct bpf_program filter;

        if (pcap_compile(handle, &filter, "udp port 5060", 0, PCAP_NETMASK_UNKNOWN) == -1)
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
        
        call_session.print_summary();

    }
    catch(const exception &e)
    {
       cout<<"ERROR:PcapReader::read_pcap" <<endl;
    }
    
}