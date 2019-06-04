#ifndef _NAVIGATION_H
#define _NAVIGATION_H
#include "Player.h"
#include "Controller.h"
#include "MemoryScanner.h"

class Navigation {

    Controller cont = NULL;

    Player* p1;
    Player* p2;

    MemoryScanner* memory;

    void tilt_right();

    void tilt_left();

    void tilt_up();

    void tilt_down();

public:

    Navigation(Controller& c, MemoryScanner* mem);
    //later have it take 2 arguments of what you want to find
    //first being the character, second the stage
    bool FindPos();
};


#endif