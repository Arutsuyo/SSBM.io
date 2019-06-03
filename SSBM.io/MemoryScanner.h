#ifndef _MEMORYSCANNER_H
#define _MEMORYSCANNER_H
#include <string>
#include "player.h"

class MemoryScanner {

public:

    MemoryScanner(std::string s);

    int UpdatedFrame();

private:
    Player p1;
    Player p2;

    void init_socket();
    std::string userPath;
    int socketfd = -1;

    void print();

    // 0: Player1 1: Player2
    Player GetPlayer(bool pl);

};



#endif