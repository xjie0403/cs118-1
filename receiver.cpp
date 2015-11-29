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
#include <sstream>
#include <fstream>

using namespace std;


int main(int argc,char* argv[])
{
	Protocol obj;
	
	cout<<argc<<endl;
	for (int n=0; n<6;n++)
		{
			cout<<argv[n]<<endl;
		}
	if (argc!=6)
	{	
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
	
	cout<<obj.set_client(hostname,port)<<endl;
	
	
	//cout<<obj.set_client("127.0.0.1", 23333) << endl
		cout<<obj.rtp_connect(filename)<<endl;
	string message;
	cout << obj.rtp_read(message) << endl;
	if (message == "wrong file name!")
			{
				cout<<message<<endl;
			}
	else{
	
		string filePath=("./"+filename);
		ofstream file(filePath.c_str());
	
		file<<message<<endl;
	
	//cout << message << endl;
	
	return 0;
	}
}




