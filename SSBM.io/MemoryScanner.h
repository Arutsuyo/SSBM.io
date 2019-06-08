#ifndef _MEMORYSCANNER_H
#define _MEMORYSCANNER_H
#include <string>
#include "Player.h"
#include "Controller.h"

class MemoryScanner {

	Player p1;
	Player p2;

	int socketfd = -1;

	std::string userPath;

	bool init_socket();

	bool in_game;

	int current_stage = -1;

public:
	bool success = false;

	// 0: Player1 1: Player2
	Player GetPlayer(bool pl);
	int UpdatedFrame();
	bool print();

	int CurrentStage() { return this->current_stage; }
	MemoryScanner(std::string s);
	~MemoryScanner();


};

#endif