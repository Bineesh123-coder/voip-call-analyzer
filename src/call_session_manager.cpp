#include "call_session_manager.h"
#include <iostream>

void CallSessionManager::process_sip(const SIPMessage &msg)
{
    std::string id = msg.call_id;

    if(call_sessions.find(id) == call_sessions.end())
    {
        CallSession session;

        session.call_id = id;
        session.caller = msg.caller;
        session.callee = msg.callee;

        call_sessions[id] = session;
    }

    CallSession &session = call_sessions[id];

    session.sip_packets++;

    if(msg.method == "INVITE")
        session.invite_seen = true;

    if(msg.method == "BYE")
        session.bye_seen = true;
}

void CallSessionManager::print_summary()
{
    std::cout << "\n===== CALL SUMMARY =====\n";

    for(auto &e : call_sessions)
    {
        auto &s = e.second;

        std::cout << "Call-ID: " << s.call_id << std::endl;
        std::cout << "Caller : " << s.caller << std::endl;
        std::cout << "Callee : " << s.callee << std::endl;
        std::cout << "Packets: " << s.sip_packets << std::endl;
        std::cout << "INVITE : " << (s.invite_seen ? "Yes":"No") << std::endl;
        std::cout << "BYE    : " << (s.bye_seen ? "Yes":"No") << std::endl;
        std::cout << "--------------------------\n";
    }
}