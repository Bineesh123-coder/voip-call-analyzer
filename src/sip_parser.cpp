#include "sip_parser.h"
#include <sstream>

using namespace std;

Logger& slogger = Logger::getInstance();

bool SIPParser::parse(const std::string &data, SIPMessage &msg)
{   
    try{

        std::istringstream stream(data);
        std::string line;

        while(std::getline(stream, line))
        {   
            // In sip_parser.cpp — inside while(getline)

            if (line.find("SIP/2.0") == 0) {
                // Status line
                std::istringstream iss(line);
                std::string version, code_str;
                iss >> version >> code_str;
                try {
                    int code = std::stoi(code_str);
                    msg.status_code = code;
                    if (code >= 200 && code < 300) {
                        msg.method = "200_OK";  // temporary marker
                    }
                } catch(...) {}
            }
            else if (line.find("INVITE sip:") == 0 || line.find("INVITE ") == 0) {
                msg.method = "INVITE";
            }
            else if (line.find("BYE sip:") == 0 || line.find("BYE ") == 0) {
                msg.method = "BYE";
            }
        
            if (line.rfind("From:", 0) == 0) {
                    size_t start = line.find("sip:");
                    size_t end   = line.find("@", start);
                    if (start != std::string::npos && end != std::string::npos) {
                        msg.caller = line.substr(start + 4, end - start - 4);
                    }
                }
                else if (line.rfind("To:", 0) == 0) {
                    size_t start = line.find("sip:");
                    size_t end   = line.find("@", start);
                    if (start != std::string::npos && end != std::string::npos) {
                        msg.callee = line.substr(start + 4, end - start - 4);
                    }
                }
                else if (line.rfind("Call-ID:", 0) == 0 || line.rfind("i:", 0) == 0) {
                    size_t colon = line.find(':');
                    if (colon != std::string::npos) {
                        msg.call_id = line.substr(colon + 1);
                        // trim
                        msg.call_id.erase(0, msg.call_id.find_first_not_of(" \t"));
                    }
                }
                else if (line.rfind("CSeq:", 0) == 0) {
                    // optional - can store method from here too
                }

                // ─────────────── SDP ───────────────
                if (line.find("m=audio") != std::string::npos) {
                    std::istringstream ss(line);
                    std::string dummy;
                    int port = 0;
                    std::string proto;
                    int payload_type;

                    ss >> dummy >> port >> proto;           // m=audio 12345 RTP/AVP ...
                    if (ss >> payload_type) {               // first payload type
                        msg.rtp_port      = port;
                        msg.payload_type  = payload_type;   // store it
                    }
                }
                else if (line.rfind("a=rtpmap:", 0) == 0) {
                    // optional: parse codec name
                }
            }

        return !msg.method.empty();

    }
    catch(const exception &e)
    {
        cout<<"ERROR:pSIPParser::parse"<<e.what() <<endl;
        slogger.WriteLog("ERROR:SIPParser::parse:"+std::string(e.what()));
    }
    return false;
}