#pragma once

#ifndef _HANDLER_
#define _HANDLER_

#include "Config.h"
#include "Controller.h"

class Handler
{
    static std::string _ssbmisoLocs[];
    static int _isoidx;

    // Important Locations
    std::string dolphinUser;
    std::string dolphinShared;

    bool initialized = false;
    int pid = -1;

    Config* cfg = NULL;
    Controller* ctrl = NULL;

    std::string getPipePath(std::string id);
public:

    bool StartDolphin();
    void KillDolphin();
    bool WaitForDolphinClose();

    Controller* getController();

    bool IsInitialized();

    Handler(VsType vs);
    ~Handler();
};


#endif