#pragma once
#include <string>

class Config
{
public:
    static enum vsType
    {
        Self = 0,
        CPU = 1,
        Human = 2
    };

    Config(vsType pType, std::string dPath, std::string isoPath);
    ~Config();

    std::string getConfig();

    void setPlayType(vsType typ);
    void setISOLocation(std::string path);
    void setDolphinLocation(std::string path);
private:
    std::string _dolphinLoc = "";
    std::string _ssbmisoLoc = "";

    bool _dual_core;
    bool _gfx;
    bool _fullscreen;

    vsType _vs;
};

