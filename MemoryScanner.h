#ifndef _MEMORYSCANNER_H
#define _MEMORYSCANNER_H
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

	MemoryScanner();


	int UpdatedFrame();

private:
	player p1;
	player p2;

	void init_socket();

	int socketfd = -1;

	void readytoprint();

};



#endif