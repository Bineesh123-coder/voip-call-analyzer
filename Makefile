CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LIBS = -lpcap

SRC = src/main.cpp src/pcap_reader.cpp src/sip_parser.cpp src/call_session_manager.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -Iinclude -o voip_analyzer $(LIBS)

clean:
	rm -f voip_analyzer