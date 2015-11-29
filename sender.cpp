#include <iostream>
#include <vector>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "Packet.h"
#include "Protocol.h"
#include <string>
#include <fstream>
#include <streambuf>
using namespace std;

int main(int argc, char* argv[]) {

	if (argc!=5 && argc!=2)
	{	
		cout<<"Wrong format!"<<endl;
		return -1;
	}
	Protocol obj;
	int port;
	port = atoi(argv[1]);
	if (argc == 5)
	{
		int CW = atoi(argv[2]);
		string pl = argv[3];
		string pc = argv[4];
		obj.set_Pl(atof(pl.c_str()));
		obj.set_Pc(atof(pc.c_str()));
		obj.set_window_size(CW);
	}
	cout<<obj.set_server(port)<<endl; 
	while (1) {
		string filename;
		obj.rtp_accept(filename);
		cout << "Filename: " << filename << endl;
		string filePath=("./"+filename);
		ifstream file(filePath.c_str(),ios::in);
		
		if(!file)
		{
			cout<<"file not found... = =b please check the file name"<<endl;
			obj.rtp_send("X");	
			continue;
	
		}
		istreambuf_iterator<char> beg(file),end;	
		string bufstr(beg,end);
		cout << obj.rtp_send(bufstr) << endl;
		cout << "Done" << endl;
		}
}
