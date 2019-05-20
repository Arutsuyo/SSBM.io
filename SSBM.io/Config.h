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

    Config(vsType vType, std::string dPath, std::string isoPath);
    ~Config();

    std::string getConfig();
    std::string getPipeConfig(int player, int pipe);

    bool IsInitialized();
private:
    std::string _dolphinLoc = "";
    std::string _ssbmisoLoc = "";

    bool _dual_core;
    bool _gfx;
    bool _fullscreen;

    vsType _vs;
};

