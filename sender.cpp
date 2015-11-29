#include <iostream>
#include <fstream>
#include <streambuf>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "Packet.h"
#include "Protocol.h"

using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 5) {
		cout<<"Wrong format!"<<endl;
		return -1;
	}

	Protocol obj;
	int port;
	port = atoi(argv[1]);
	int CW = atoi(argv[2]);
	string pl = argv[3];
	string pc = argv[4];
	obj.set_Pl(atof(pl.c_str()));
	obj.set_Pc(atof(pc.c_str()));
	obj.set_window_size(CW);
	obj.set_server(port);
	
	while (1) {
		string filename;
		obj.rtp_accept(filename);
		cout << "Filename: " << filename << endl;
		
		string filePath=("./"+filename);
		ifstream file(filePath.c_str(),ios::in);
		
		if (!file) {
			cout << "file not found... = =b please check the file name" << endl;
			obj.rtp_send("wrong file name!");	
			continue;
		}

		istreambuf_iterator<char> beg(file),end;	
		string bufstr(beg,end);
		obj.rtp_send(bufstr);

		cout << "Transfered successfully" << endl;
	}
	
	return 0;
}
