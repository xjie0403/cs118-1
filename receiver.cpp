#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Packet.h"
#include "Protocol.h"

using namespace std;


int main(int argc,char* argv[]) {
	Protocol obj;

	if (argc != 6) {
		cout<<"Wrong format!"<<endl;
		return -1;
	}

	string hostname;
	int port;
	string filename;	

	hostname = argv[1];
	port = atoi(argv[2]);
	filename = argv[3];
	string pl=argv[4];
	string pc=argv[5];
	obj.set_Pl(atof(pl.c_str()));
	obj.set_Pc(atof(pc.c_str()));
	
	obj.set_client(hostname,port);
	
	obj.rtp_connect(filename);
	string message;
	obj.rtp_read(message);

	if (message == "wrong file name!") {
		cout << message << endl;
	} else {
		string filePath=("./"+filename);
		ofstream file(filePath.c_str());
		file << message;
		file.close();
	}

	return 0;
}




