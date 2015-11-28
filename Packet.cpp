#include "Packet.h"

Packet::Packet() {
	SEQ = ACK = checksum = 0;
	SYN = FIN = false;
	content = "";
}

Packet::Packet(char* buffer) {
	memcpy(&SEQ, buffer, sizeof(int));
	memcpy(&ACK, buffer + sizeof(int), sizeof(int));
	memcpy(&checksum, buffer + 2 * sizeof(int), sizeof(int));
	memcpy(&length, buffer + 3 * sizeof(int), sizeof(int));
	memcpy(&SYN, buffer + 4 * sizeof(int), sizeof(bool));
	memcpy(&FIN, buffer + 4 * sizeof(int) + sizeof(bool), sizeof(bool));

	int startByte = 4 * sizeof(int) + 2 * sizeof(bool);

	content = "";

	for (int i = startByte; i < min(BUFFSIZE, startByte + length); i++) {
		char ch;
		memcpy(&ch, buffer + i, sizeof(char));

		content = content + ch;
	}
}

bool Packet::to_buffer(char* buffer) {
	checksum = calc_checksum();
	length = content.size();

	memset(buffer, 0, BUFFSIZE);
	memcpy(buffer, &SEQ, sizeof(int));
	memcpy(buffer + sizeof(int), &ACK, sizeof(int));
	memcpy(buffer + 2 * sizeof(int), &checksum, sizeof(int));
	memcpy(buffer + 3 * sizeof(int), &length, sizeof(int));
	memcpy(buffer + 4 * sizeof(int), &SYN, sizeof(bool));
	memcpy(buffer + 4 * sizeof(int) + sizeof(bool), &FIN, sizeof(bool));

	int startByte = 4 * sizeof(int) + 2 * sizeof(bool);

	if (content.size() > BUFFSIZE - startByte) {
		cerr << "to_buffer error: packet's content is too large" << endl;
		return false;
	}

	for (int i = 0; i < content.size(); i++) {
		char ch = content[i];
		memcpy(buffer + startByte + i, &ch, sizeof(char));
	}

	return true;
}

void Packet::corrupt_buffer(char* buffer) {
	int startByte = 4 * sizeof(int) + 2 * sizeof(bool);
	int rnd = rand() % 256;
	memcpy(buffer + startByte, &rnd, sizeof(int));
}

bool Packet::is_valid() {
	int crc = calc_checksum();

	if (crc == checksum)
		return true;
	else
		return false;
}

bool Packet::is_content_full() {
	int startByte = 4 * sizeof(int) + 2 * sizeof(bool);

	if (content.size() >= BUFFSIZE - startByte)
		return true;
	else
		return false;
}

int Packet::calc_checksum() {
	int crc = 0;
	for (int i = 0; i < content.size(); i++) {
		crc = crc ^ (unsigned short)content[i] << 8;
		
		int j = 8;
		do {
			if (crc & 0x8000)
				crc = crc << 1 ^ 0x1021;
			else
				crc = crc << 1;
		} while (--j);
	}

	return crc;
}