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
        std::cout << "Codec : " << s.codec<<" "<<s.codec_name << std::endl;
        std::cout << "Packets: " << s.sip_packets << std::endl;
        std::cout << "INVITE : " << (s.invite_seen ? "Yes":"No") << std::endl;
        std::cout << "BYE    : " << (s.bye_seen ? "Yes":"No") << std::endl;
        std::cout << "SIP Packets : " << s.sip_packets << endl;
        std::cout << "RTP Packets : " << s.rtp_packets << endl;
        std::cout << "Packet lose : " << s.packet_loss << std::endl;
        double jitter_ms = (s.jitter / 8000.0) * 1000;
        std::cout<<std::fixed<<std::setprecision(2)<< "Jitter : " << jitter_ms<<" ms" << std::endl;

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

void CallSessionManager::process_rtp(const std::string& call_id, const u_char* payload, time_t timestamp)
{
    CallSession &session = call_sessions[call_id];

    session.rtp_packets++;

    if(session.first_rtp_time == 0)
        session.first_rtp_time = timestamp;

    session.last_rtp_time = timestamp;

    // ----- RTP HEADER PARSING -----

    uint8_t payload_type = payload[1] & 0x7F;

    uint16_t seq =
        (payload[2] << 8) |
        payload[3];

    uint32_t rtp_timestamp =
        (payload[4] << 24) |
        (payload[5] << 16) |
        (payload[6] << 8)  |
        payload[7];

    // Store codec
    session.codec = payload_type;

    // Packet loss detection
    if(session.last_seq != 0)
    {
        int diff = seq - session.last_seq;

        if(diff > 1)
            session.packet_loss += (diff - 1);
    }

    session.last_seq = seq;

    // Simple jitter calculation
    if(session.last_timestamp != 0)
    {
        int transit = rtp_timestamp - session.last_timestamp;

        int d = abs(transit);

        session.jitter += (d - session.jitter) / 16;
    }

    session.last_timestamp = rtp_timestamp;

    if(session.codec == 0)
    session.codec_name = "G711 PCMU";
    else if(session.codec == 8)
        session.codec_name = "G711 PCMA";
    else if(session.codec == 18)
        session.codec_name = "G729";
    else
        session.codec_name = "Dynamic";
}