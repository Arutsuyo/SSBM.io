#pragma once
#ifndef _CONFIG_
#define _CONFIG_

#include <string>
#include "Types.h"

class Config
{
    bool initialized;

public:
    Config(VsType vType);
    ~Config();

    std::string getConfig();
    std::string getPipeConfig(int pipe);
    std::string getPipeLoc(int pipe);

    bool IsInitialized();
private:
    bool _dual_core;
    bool _gfx;
    bool _fullscreen;

    VsType _vs;
};


#endif