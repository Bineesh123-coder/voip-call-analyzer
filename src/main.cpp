#include "pcap_reader.h"
#include "logger.h"
#include <iostream>

int main(int argc, char* argv[])
{   
    Logger::getInstance();   // initializes the logger (creates logs/ folder)
    PcapReader reader;

    if(argc < 3)
    {
        std::cout << "Usage:\n";
        std::cout << "./voip_analyzer -r file.pcap\n";
        std::cout << "./voip_analyzer -i interface\n";
        return 0;
    }

    std::string mode = argv[1];

    if(mode == "-r")
    {
        reader.read_pcap(argv[2]);
    }
    else if(mode == "-i")
    {
        reader.capture_live(argv[2]);
    }

    return 0;
}


