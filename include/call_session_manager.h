#ifndef CALL_SESSION_MANAGER_H
#define CALL_SESSION_MANAGER_H

#include <unordered_map>
#include <string>
#include "sip_parser.h"
#include <iomanip>

struct CallSession
{
    std::string call_id;
    std::string caller;
    std::string callee;

    bool invite_seen = false;
    bool bye_seen = false;


    int sip_packets = 0;
    int rtp_packets = 0;

    time_t start_time = 0;
    time_t end_time = 0;

    time_t first_rtp_time = 0;
    time_t last_rtp_time = 0;

    uint16_t last_seq = 0;
    int packet_loss = 0;

    uint32_t last_timestamp = 0;

    double jitter = 0;

    int codec = -1;

    std::string codec_name ="";

    size_t rtp_bytes = 0;

};

class CallSessionManager
{

public:

   

    std::unordered_map<std::string, CallSession> call_sessions;

    std::unordered_map<int,std::string> rtp_port_map;

    void process_sip(const SIPMessage &msg,time_t timestamp);

    bool is_rtp(const u_char* payload);
    
    void process_rtp(const std::string& call_id,const u_char* payload,int payload_len,time_t timestamp);

    void print_summary();

    void find_MOS_quality(const int packet_loss,double jitter);
};

#endif