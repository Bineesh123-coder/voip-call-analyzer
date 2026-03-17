#ifndef PCAP_READER_H
#define PCAP_READER_H

#include <pcap.h>
#include <iostream>
#include <exception>

using namespace std;


class PcapReader
{
public:
    void read_pcap(const std::string &file);
    void capture_live(const std::string& interface);
};

#endif