#pragma once
#include "Config.h"
#include "Controller.h"

class Handler
{
    int pid;

    Config* cfg;
    Controller* ctrl;
public:

    bool StartDolphin();
    bool KillDolphin();
    bool WaitForDolphinClose();

    Controller* getController();

    bool IsInitialized();

    Handler(int numAI, int numCPU, int numHuman);
    ~Handler();
};

