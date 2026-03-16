#ifndef SIP_PARSER_H
#define SIP_PARSER_H

#include <string>

struct SIPMessage
{
    std::string method;
    std::string caller;
    std::string callee;
    std::string call_id;
    int  rtp_port = 0;
};

class SIPParser
{
public:
    bool parse(const std::string &data, SIPMessage &msg);
};

#endif