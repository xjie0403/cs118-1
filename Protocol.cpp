#include "Protocol.h"

Protocol::Protocol() {
	srand(time(NULL));

	sndpkt.clear();
	sndpktPtr = 0;
	timer = 0;
	mode = 0;
	windowsSize = 10;

	Pl = Pc = 0;

	dstLen = sizeof(dstAddr);
}

bool Protocol::set_server(int port) {
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		cerr << "Can't create a socket" << endl;
		return false;
	}

	struct sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(fd, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1) {
		cerr << "set_server: binding failed" << endl;
		return false;
	}

	mode = 1;

	return true;
}

bool Protocol::set_client(string hostname, int port) {
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		cerr << "Can't create a socket" << endl;
		return false;
	}

	struct hostent *server;
	server = gethostbyname(hostname.c_str());

	if (server == NULL) {
		cerr << "gethostname error" << endl;
		return false;
	}

	memset(&dstAddr, 0, sizeof(sockaddr_in));
	dstAddr.sin_family = AF_INET;
	dstAddr.sin_port = htons(port);
	if (inet_aton(hostname.c_str(), &dstAddr.sin_addr)==0) {
	}

	struct sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(33333);
    
	if (bind(fd, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) < 0) {
		cerr << "bind error" << endl;
		return false;
	}

	mode = 2;

	return true;
}

bool Protocol::rtp_connect(string filename) {
	if (mode != 2) {
		cerr << "Illegal interface call: not under client mode" << endl;
		return false;
	}

	Packet synPkt;
	synPkt.SYN = true;
	synPkt.content = filename;

	char buffer[BUFFSIZE];

	synPkt.to_buffer(buffer);

	dstLen = sizeof(dstAddr);
	if (sendto(fd, buffer, BUFFSIZE, 0, (struct sockaddr *)&dstAddr, dstLen) < 0) {
		cerr << "rtp_connect failed" << endl;

		return false;
	}

	return true;
}

void Protocol::rtp_accept(string &filename) {
	if (mode != 1) {
		cerr << "Illegal interface call: not under server mode" << endl;
		return;
	}

	char buffer[BUFFSIZE];

	while (1) {
		// wait for a SYN packet
		if (recvfrom(fd, buffer, BUFFSIZE, 0, (struct sockaddr*)&dstAddr, &dstLen) < 0) {
			cerr << "accept error" << endl;
			continue;
		}

		Packet pkt(buffer);

		if (!pkt.is_valid()) {
			cerr << "accept error, a non valid packet" << endl;
			continue;
		}

		if (pkt.SYN == false) {
			cerr << "accept error, a non SYN packet" << endl;
			continue;
		}

		filename = pkt.content;
		
		ourSEQ = rand() % 1000;

		// construct SYN + ACK packet for the intializing connection
		Packet ackPkt;
		ackPkt.SYN = true;
		ackPkt.ACK = -1;
		add_and_send(ackPkt);

		break;
	}
}

bool Protocol::rtp_read(string &returnMessage) {
	if (mode != 2) {
		cerr << "Illegal interface call: not under client mode" << endl;
		return false;
	}

	returnMessage = "";

	time(&timer);
	char buffer[BUFFSIZE];

	bool isSYN = false;

	// wait for first SYN
	while (time(NULL) <= timer + 3 * CFG_TIMEOUT) {
		if (recvfrom(fd, buffer, sizeof(buffer), MSG_DONTWAIT, NULL, NULL) < 0) {
			continue;
		}

		Packet synPkt(buffer);
		if (!synPkt.is_valid()) {
			cerr << "Not a valid SYN" << endl;
			continue;
		}

		if (synPkt.SYN == false) {
			cerr << "Not a SYN packet" << endl;
			continue;
		}

		itsSEQ = synPkt.SEQ + 1;
		isSYN = true;

		Packet ack;
 		ack.ACK = synPkt.SEQ;
 		ack.to_buffer(buffer);

 		if (sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0) {
 			cerr << "First ACK failed" << endl;
 		}

		break;
	}

	if (!isSYN) {
		cerr << "Timeout: can't established connection" << endl;
		return false;
	}

	cerr << "Received the SYN" << endl;

	while (1) {
		if (recvfrom(fd, buffer, sizeof(buffer), MSG_DONTWAIT, NULL, NULL) < 0) {
			continue;
		}

		Packet pkt(buffer);
		if (!pkt.is_valid()) {
			cerr << "Received a corrupted packet" << endl;
			continue;
		}

		if (pkt.SEQ > itsSEQ) {
			cerr << "Received a disordered packet" << endl;
			continue;
 		} else if (pkt.SEQ < itsSEQ) {
 			cerr << "Lost a ACK before, try to re-ACK" << endl;

 			Packet ack;
 			ack.ACK = pkt.SEQ;

 			ack.to_buffer(buffer);

 			if (sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0) {
 				cerr << "Re-ACK failed" << endl;
 			}

 			continue;
 		}

 		if (pkt.FIN) {
 			// last step
 			cerr << "FIN received" << endl;

 			Packet ack;
 			ack.ACK = pkt.SEQ;

 			ack.to_buffer(buffer);

 			if (sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0) {
 				cerr << "ACK of FIN failed" << endl;
 				continue;
 			}

 			// Last Wait
 			cerr << "Enter " << CFG_LASTWAIT << "-seconds Waiting Phase" << endl;
 			time(&timer);
 			while (time(NULL) < timer + CFG_LASTWAIT) {
 				if (recvfrom(fd, buffer, sizeof(buffer), MSG_DONTWAIT, NULL, NULL) < 0) {
					continue;
				}

				Packet newPacket(buffer);
				if (!newPacket.is_valid()) {
					cerr << "Received a corrupted packet" << endl;
					continue;
				}

				if (newPacket.FIN) {
					ack.to_buffer(buffer);

					cerr << "Re-send last ACK packet" << endl;

 					if (sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0) {
 						cerr << "ACK of FIN failed" << endl;
 						continue;
 					}
				}
 			}

 			return true;
 		}

 		returnMessage += pkt.content;
 		itsSEQ++;

 		Packet ack;
 		ack.ACK = pkt.SEQ;

 		ack.to_buffer(buffer);

 		if (sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0) {
 			cerr << "ACK failed" << endl;
 		}
	}
}

bool Protocol::rtp_send(string sendMessage) {
	if (mode != 1) {
		cerr << "Illegal interface call: not under server mode" << endl;
		return false;
	}

	Packet pkt;

	for (int i = 0; i <= sendMessage.size(); i++) {
		if (i == sendMessage.size() || pkt.is_content_full()) {
			add_and_send(pkt);
			pkt.content = "";
		}

		if (i < sendMessage.size()) {
			pkt.content += sendMessage[i];
		}
	}

	Packet fin;
	fin.FIN = true;
	add_and_send(fin);

	while (!take_care_send());

	return true;
}

bool Protocol::add_and_send(Packet pkt) {
	while (sndpkt.size() - sndpktPtr >= CFG_WINDOWSIZE) {
		take_care_send();
	}

	pkt.SEQ = ourSEQ++;
	sndpkt.push_back(pkt);

	if (timer == 0) {
		time(&timer);
	}

	char buffer[BUFFSIZE];

	pkt.to_buffer(buffer);

	// Under possiblity Pc to corrupt packet
	if (under_possibility(Pc))
		pkt.corrupt_buffer(buffer);

	// Under possiblity Pl to lose packet
	if (under_possibility(Pl))
		return true;

	if (sendto(fd, buffer, BUFFSIZE, 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0) {
		cerr << "send packet error: add_and_send()" << endl;
		return false;
	}

	return true;
}

/**
 @return:
 	true: all sender packets ACKed
 	false: exists some packets timeout
*/
bool Protocol::take_care_send() {
	if (sndpktPtr >= sndpkt.size())
		return true;

	char buffer[BUFFSIZE];
	do {
		// loop and wait
		if (sndpktPtr == sndpkt.size()) {
			timer = 0;
			return true;
		}

		if (recvfrom(fd, buffer, sizeof(buffer), MSG_DONTWAIT, (struct sockaddr*)&dstAddr, &dstLen) < 0) {
			continue;
		}

		Packet pkt(buffer);
		
		if (!pkt.is_valid()) {
			cerr << "invalid packet" << endl;
			continue;
		}

		if (pkt.ACK == sndpkt[sndpktPtr].SEQ) {
			// ACK successfully
			sndpktPtr++;
		} else {
			// out of order ACK, drop
			continue;
		}
		
	} while (time(NULL) <= timer + CFG_TIMEOUT);

	if (sndpktPtr == sndpkt.size())
		return true;

	// timeout, re-send all packets in the sndpkt
	cerr << "Sender timeout! Re-send all packets" << " " << sndpktPtr << " " << sndpkt.size() << endl;

	time(&timer);
	for (int i = sndpktPtr; i < sndpkt.size(); i++) {
		sndpkt[i].to_buffer(buffer);

		// Under possiblity Pc to corrupt packet
		if (under_possibility(Pc))
			sndpkt[i].corrupt_buffer(buffer);

		// Under possiblity Pl to lose packet
		if (under_possibility(Pl))
			continue;

		if (sendto(fd, buffer, sizeof(buffer), 0, (struct sockaddr *)&dstAddr, sizeof(dstAddr)) < 0) {
			cerr << "re-send packer error: take_care_send()" << endl;
			return false;
		}
	}

	return false;
}

void Protocol::set_Pl(float Pl) {
	this->Pl = Pl;
}

void Protocol::set_Pc(float Pc) {
	this->Pc = Pc;
}

void Protocol::set_window_size(int windowsSize) {
	this->windowsSize = windowsSize;
}

bool Protocol::under_possibility(float possibility) {
	possibility = 100 - possibility * 100;

	if (rand() % 100 >= possibility)
		return true;
	else
		return false;
}
