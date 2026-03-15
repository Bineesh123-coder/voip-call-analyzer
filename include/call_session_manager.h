#ifndef CALL_SESSION_MANAGER_H
#define CALL_SESSION_MANAGER_H

#include <unordered_map>
#include <string>
#include "sip_parser.h"

struct CallSession
{
    std::string call_id;
    std::string caller;
    std::string callee;

    bool invite_seen = false;
    bool bye_seen = false;

    int sip_packets = 0;
};

class CallSessionManager
{
private:

    std::unordered_map<std::string, CallSession> call_sessions;

public:

    void process_sip(const SIPMessage &msg);

    void print_summary();
};

#endif