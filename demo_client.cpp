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

using namespace std;


int main()
{
	Protocol obj;
	string command;
	string str[4];
	string hostname;
	int port;
	string filename;
	
	int i = 0;
	cout<<"Please input receiver command"<<endl;
	cin>>command;
	istringstream iss(command);
	while(!istr.eof())
	{
		if (i>3)
		{
			cout<<"wrong command format"<<endl;
			return 0;
		}
		istr>>str[i];
		cout<<str[i]<<endl;
		i++;
	}
	if (str[0]!="receiver")
		{
			cout<<"please input the right comand"<<endl;
			return 0;
		}
	hostname = str[1];
	port= atoi(str[2].c_str());
	filename = str[3];
	
	cout<<obj.set_client(hostname,port)<<endl;
	
	//cout<<obj.set_client("127.0.0.1", 23333) << endl
	//	obj.rtp_connect("gan.cpp");
	//string message = "";
	//cout << obj.rtp_read(message) << endl;
	//cout << message << endl;
	
	return 0;
}
