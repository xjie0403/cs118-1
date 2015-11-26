all: server client
	
server: demo_server.cpp Protocol.cpp Protocol.h Packet.cpp Packet.h
	g++ -o $@ demo_server.cpp Protocol.cpp Packet.cpp

client: demo_client.cpp Protocol.cpp Protocol.h Packet.cpp Packet.h
	g++ -o $@ demo_client.cpp Protocol.cpp Packet.cpp
