#pragma once
#include "Config.h"
#include "Controller.h"

class Handler
{
    static std::string dolphinexe;
    static std::string dolphinuser;
    static std::string _dolphinLoc;
    static std::string _ssbmisoLoc;
    std::string _customINI;

    bool initialized = false;
    int pid = -1;

    Config* cfg = NULL;
    Controller* ctrl = NULL;

public:

    bool StartDolphin();
    bool KillDolphin();
    bool WaitForDolphinClose();

    Controller* getController();

    bool IsInitialized();

    Handler(int numAI, int numCPU, int numHuman);
    ~Handler();
};

