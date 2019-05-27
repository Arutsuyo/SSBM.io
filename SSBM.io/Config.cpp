#include "Config.h"

#include <fstream>
#include <iostream>

#define _GFX_BACKEND "D3D"
#define _DUAL_CORE_DEFAULT 1
#define _GFX_DEFAULT false
#define _FULLSCREEN_DEFAULT false

using namespace std;

string AIController =
"Buttons / A = `Button A`\n"
"Buttons / B = `Button B`\n"
"Buttons / X = `Button X`\n"
"Buttons / Y = `Button Y`\n"
"Buttons / Z = `Button Z`\n"
"Buttons / Start = `Button START`\n"
"D - Pad / Up = `Button D_UP`\n"
"D - Pad / Down = `Button D_DOWN`\n"
"D - Pad / Left = `Button D_LEFT`\n"
"D - Pad / Right = `Button D_RIGHT`\n"
"Triggers / L = `Button L`\n"
"Triggers / R = `Button R`\n"
"Main Stick / Up = `Axis MAIN Y - `\n"
"Main Stick / Down = `Axis MAIN Y + `\n"
"Main Stick / Left = `Axis MAIN X - `\n"
"Main Stick / Right = `Axis MAIN X + `\n"
"C - Stick / Up = `Axis C Y - `\n"
"C - Stick / Down = `Axis C Y + `\n"
"C - Stick / Left = `Axis C X - `\n"
"C - Stick / Right = `Axis C X + `\n";

Config::Config(VsType vType = VsType::Self)
{
    // Set the vs type
    _vs = vType;

    // Set default values
    _dual_core = _DUAL_CORE_DEFAULT;

    // Do we need to render?
    if (_vs == VsType::Human)
    {
        _gfx = true;
        _fullscreen = true;
    }
    else
    {
        _gfx = _GFX_DEFAULT;
        _fullscreen = _FULLSCREEN_DEFAULT;
    }

    initialized = true;
}

string Config::getPipeConfig(int pipe)
{
    char buff[256];
    sprintf(buff, "[GCPad1]\nDevice = Pipe/1/AI%d\n", pipe);
    string pipeOut(buff);
    pipeOut += AIController;
    return pipeOut;
}

string Config::getPipeLoc(int pipe)
{
    char buff[256];
    sprintf(buff, "Pipe/pipe%d\n", pipe);
    string pipeOut(buff);
    return pipeOut;
}

bool Config::IsInitialized()
{
    return initialized;
}

string Config::getConfig()
{
    char buff[256];
    string output = string();
    output += 
        "[Core]\n"
        "DSPHLE = True\n"
        "CPUCore = 1\n"
        "OverclockEnable = False\n";

    // True or False to enable and disable "Dual Core" mode respectively.
    sprintf(buff, "CPUThread = %s\n", _dual_core ? "True" : "False");
    output += buff;

    // Graphics Backend, Defaults Direct3D on WIN32 or OpenGL on other platforms.
    sprintf(buff, "GFXBackend = %s\n", _gfx ? _GFX_BACKEND : "Null");
    output += buff;

    // Keep this section last as it transitions out of [Core]
    if (_gfx)
    {
        output +=
            "EmulationSpeed = 1\n"
            "[Video_Settings]\n"
            "InternalResolution = 3\n"
            "ShaderCompilationMode = 2\n"
            "MSAA = 3\n"
            "SSAA = False\n"
            "[Video_Enhancements]\n"
            "MaxAnisotropy = 3\n"
            "[DSP]\n"
            "Backend = XAudio2\n";
    }
    else
    {
        output +=
            "EmulationSpeed = 0\n";
    }

    return output;
}

Config::~Config()
{
}
