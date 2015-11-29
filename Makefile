all: sender receiver
	
sender: sender.cpp Protocol.cpp Protocol.h Packet.cpp Packet.h
	g++ -o $@ sender.cpp Protocol.cpp Packet.cpp

receiver: receiver.cpp Protocol.cpp Protocol.h Packet.cpp Packet.h
	g++ -o $@ receiver.cpp Protocol.cpp Packet.cpp
