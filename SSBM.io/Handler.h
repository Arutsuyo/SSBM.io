#pragma once

#ifndef _HANDLER_
#define _HANDLER_

#include "Config.h"
#include "Controller.h"

class Handler
{
    static std::string dolphinexe;
    static std::string dolphinuser;
    static std::string _dolphinLoc;
    static std::string _ssbmisoLoc;
    static std::string _customINI;

    bool initialized = false;
    int pid = -1;

    Config* cfg = NULL;
    Controller* ctrl = NULL;

public:

    bool StartDolphin();
    void KillDolphin();
    bool WaitForDolphinClose();

    Controller* getController();

    bool IsInitialized();

    Handler();
    ~Handler();
};


#endif