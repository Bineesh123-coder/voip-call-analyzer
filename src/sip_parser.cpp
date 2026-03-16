#include "sip_parser.h"
#include <sstream>

using namespace std;

bool SIPParser::parse(const std::string &data, SIPMessage &msg)
{
    std::istringstream stream(data);
    std::string line;

    while(std::getline(stream, line))
    {
        if(line.find("INVITE") != std::string::npos)
            msg.method = "INVITE";

        if(line.find("BYE") != std::string::npos)
            msg.method = "BYE";

        if(line.find("From:") != std::string::npos)
        {
            size_t start = line.find("sip:");
            size_t end = line.find("@");

            if(start != std::string::npos && end != std::string::npos)
                msg.caller = line.substr(start + 4, end - start - 4);
        }

        if(line.find("To:") != std::string::npos)
        {
            size_t start = line.find("sip:");
            size_t end = line.find("@");

            if(start != std::string::npos && end != std::string::npos)
                msg.callee = line.substr(start + 4, end - start - 4);
        }

        if(line.find("Call-ID:") != std::string::npos)
        {
            size_t start = line.find(":");
            msg.call_id = line.substr(start + 2);
        }
        if(line.find("m=audio") != string::npos)
        {
            stringstream ss(line);
            string media;
            int port;

            ss >> media >> port;

            msg.rtp_port = port;
        }
    }

    return !msg.method.empty();
}