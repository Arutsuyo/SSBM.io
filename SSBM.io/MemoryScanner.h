#ifndef _MEMORYSCANNER_H
#define _MEMORYSCANNER_H
#include <string>
#include "Player.h"

/*
struct player{

	unsigned int health;
	int dir; //left -1 right 1
	float pos x;	
	float pos y;

};
*/

class MemoryScanner{

public:

	MemoryScanner(std::string s);


	int UpdatedFrame();

private:
	player p1;
	player p2;

	void init_socket();
	std::string p;
	int socketfd = -1;

	void readytoprint();

};



#endif