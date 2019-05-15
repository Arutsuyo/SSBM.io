#include "Config.h"

#include <fstream>
#include <iostream>

#ifdef WIN32
#define _GFX_BACKEND "D3D"
#else
#define _GFX_BACKEND "OGL"
#endif

#define _DUAL_CORE_DEFAULT 1
#define _GFX_DEFAULT false
#define _FULLSCREEN_DEFAULT false

using namespace std;

string AIController =
"[GCPad1]\n"
"Device = Pipe / 0 / pipe1\n"
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

/* Helper Functions */
inline bool exists_test(const string& name) {
    if (FILE * file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

Config::Config(vsType pType, 
    string dPath = "%dolphin%\Dolphin.exe", 
    string isoPath = "%dolphin%\iso\ssbm.gcm")
{
    // Check for existing paths
    if (!exists_test(dPath))
    {
        cerr << "Error: Path to Dolphin exec does not exist:\n"
            << dPath << endl;
    }
    else
        _dolphinLoc = dPath;

    if (!exists_test(isoPath))
    {
        cerr << "Error: Path to ssbm iso does not exist:\n"
            << isoPath << endl;
    }
    else
        _ssbmisoLoc = isoPath;

    // Set default values
    _dual_core = _DUAL_CORE_DEFAULT;
    _gfx = _GFX_DEFAULT;
    _fullscreen = _FULLSCREEN_DEFAULT;
}

Config::~Config()
{
}

std::string Config::getConfig()
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
            "MaxAnisotropy = 3"
            "[DSP]\n"
#ifdef WIN32
            "Backend = XAudio2\n"
#else
            // "Backend = OpenAL\n"
            "Backend = Pulse\n"
#endif;
            ;
    }
    else
    {
        output +=
            "EmulationSpeed = 0\n";
    }

}

void Config::setPlayType(vsType typ)
{
    _vs = typ;
}

void Config::setISOLocation(string path)
{
    if (exists_test(path))
        _ssbmisoLoc = path;
}

void Config::setDolphinLocation(string path)
{
    if (exists_test(path))
        _dolphinLoc = path;
}

