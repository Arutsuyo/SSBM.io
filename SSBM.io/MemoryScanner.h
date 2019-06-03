#ifndef _MEMORYSCANNER_H
#define _MEMORYSCANNER_H
#include <string>
#include "player.h"

class MemoryScanner {

	Player p1;
	Player p2;

	int socketfd = -1;

	std::string userPath;

	bool init_socket();
public:
	bool success = false;

	// 0: Player1 1: Player2
	Player GetPlayer(bool pl);
	bool UpdatedFrame();
	void print();

	MemoryScanner(std::string s);
	~MemoryScanner();
};

#endif