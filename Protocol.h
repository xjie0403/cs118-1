#include <iostream>
#include <vector>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Packet.h"

#ifndef H_PROTOCOL
#define H_PROTOCOL

using namespace std;

#define CFG_TIMEOUT 1
#define CFG_LASTWAIT 3
#define CFG_WINDOWSIZE 10

class Protocol {
public:
	Protocol();

	bool set_server(int port);
	bool set_client(string hostname, int port);

	bool rtp_connect(string filename);
	void rtp_accept(string &filename);

	bool rtp_read(string &returnMessage);
	bool rtp_send(string sendMessage);

	void set_Pl(float Pl);
	void set_Pc(float Pc);

private:
	int fd;
	int mode;
	int ourSEQ, itsSEQ;
	float Pl, Pc;
	struct sockaddr_in dstAddr;
	socklen_t dstLen;
	
	vector<Packet> sndpkt;
	int sndpktPtr;
	time_t timer;

	bool add_and_send(Packet pkt);
	bool take_care_send();
	bool under_possibility(float possibility);
};

#endif