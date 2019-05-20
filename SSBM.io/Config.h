#pragma once
#include <string>

class Config
{
    bool initialized;

public:
    static enum vsType
    {
        Self = 0,
        CPU = 1,
        Human = 2
    };

    Config(vsType vType);
    ~Config();

    std::string getConfig();
    std::string getPipeConfig(int pipe);
    std::string getPipeLoc(int pipe);

    bool IsInitialized();
private:
    bool _dual_core;
    bool _gfx;
    bool _fullscreen;

    vsType _vs;
};

