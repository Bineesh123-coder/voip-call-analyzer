#include "call_session_manager.h"
#include <iostream>

using namespace std;

void CallSessionManager::process_sip(const SIPMessage &msg,time_t timestamp)
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
    {
        session.invite_seen = true;
        session.start_time = timestamp;
    }
        

    if(msg.method == "BYE")
    {
        session.bye_seen = true;
        session.end_time = timestamp;
    }

    if(msg.rtp_port > 0)
    {
        rtp_port_map[msg.rtp_port] = msg.call_id;
    }
        
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
        std::cout << "SIP Packets : " << s.sip_packets << endl;
        std::cout << "RTP Packets : " << s.rtp_packets << endl;

        int duration = 0;
        if(s.start_time && s.end_time)
        {
            duration = s.end_time - s.start_time;
        }
        else if(s.first_rtp_time && s.last_rtp_time)
        {
            duration = s.last_rtp_time - s.first_rtp_time;
        }

        std::cout << "Duration : " << duration << " seconds" << endl;
                std::cout << "--------------------------\n";
    }
}


bool CallSessionManager::is_rtp(const u_char* payload)
{
    int version = (payload[0] >> 6);

    if(version == 2)
        return true;

    return false;
}

void CallSessionManager::process_rtp(const std::string& call_id, time_t timestamp)
{
    CallSession &session = call_sessions[call_id];

    session.rtp_packets++;

    if(session.first_rtp_time == 0)
        session.first_rtp_time = timestamp;

    session.last_rtp_time = timestamp;
}