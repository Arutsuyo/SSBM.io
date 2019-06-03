#ifndef _MEMORYSCANNER_H
#define _MEMORYSCANNER_H
#include <string>
#include "Player.h"

class MemoryScanner{

public:

	MemoryScanner(std::string s);

	~MemoryScanner();

	bool GetStatus(){return this->success; }

	int UpdatedFrame();

private:
	player p1;
	player p2;

	bool success = false;
	bool init_socket();
	std::string p;
	int socketfd = -1;

	void readytoprint();

};



#endif