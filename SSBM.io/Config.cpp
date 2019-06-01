#include "Config.h"

#include <fstream>
#include <iostream>

#define _GFX_BACKEND "OGL"
#define _DUAL_CORE_DEFAULT 1
#define _GFX_DEFAULT false
#define _FULLSCREEN_DEFAULT false

using namespace std;

string PlayerKeyboard =
"Device = Xlib/0/Keyboard Mouse\n"
"Buttons/A = V\n"
"Buttons/B = C\n"
"Buttons/X = X\n"
"Buttons/Y = `Button Y`\n"
"Buttons/Z = Z\n"
"Buttons/Start = Return\n"
"Main Stick/Up = Up\n"
"Main Stick/Down = Down\n"
"Main Stick/Left = Left\n"
"Main Stick/Right = Right\n"
"C-Stick/Up = `Axis C Y -`\n"
"C-Stick/Down = `Axis C Y +`\n"
"C-Stick/Left = `Axis C X -`\n"
"C-Stick/Right = `Axis C X +`\n"
"Triggers/L = space\n"
"D-Pad/Up = `Button D_UP`\n"
"D-Pad/Down = `Button D_DOWN`\n"
"D-Pad/Left = `Button D_LEFT`\n"
"D-Pad/Right = `Button D_RIGHT`\n";

string AIController =
"Buttons/A = `Button A`\n"
"Buttons/B = `Button B`\n"
"Buttons/X = `Button X`\n"
"Buttons/Y = `Button Y`\n"
"Buttons/Z = `Button Z`\n"
"Buttons/Start = `Button START`\n"
"D-Pad/Up = `Button D_UP`\n"
"D-Pad/Down = `Button D_DOWN`\n"
"D-Pad/Left = `Button D_LEFT`\n"
"D-Pad/Right = `Button D_RIGHT`\n"
"Triggers/L = `Button L`\n"
"Triggers/R = `Button R`\n"
"Main Stick/Up = `Axis MAIN Y - `\n"
"Main Stick/Down = `Axis MAIN Y + `\n"
"Main Stick/Left = `Axis MAIN X - `\n"
"Main Stick/Right = `Axis MAIN X + `\n"
"C-Stick/Up = `Axis C Y - `\n"
"C-Stick/Down = `Axis C Y + `\n"
"C-Stick/Left = `Axis C X - `\n"
"C-Stick/Right = `Axis C X + `\n";

string hotkey =
"Keys/Load State Slot 1 = `Button R`";

Config::Config(VsType vType)
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

string Config::getPlayerPipeConfig(int player)
{
    char buff[256];
    printf("CFG: Creating Keyboard Player: %d\n", player);
    sprintf(buff, "[GCPad%d]\n", player + 1);
    string pipeOut(buff);
    pipeOut += PlayerKeyboard;
    return pipeOut;
}

string Config::getAIPipeConfig(int player, int pipe_count, string id)
{
    char buff[256];
    sprintf(buff,
        "[GCPad%d]\nDevice = Pipe/%d/%s%d\n",
        player + 1,
        pipe_count,
        id.c_str(),
        pipe_count);
    string pipeOut(buff);
    pipeOut += AIController;
    return pipeOut;
}

string Config::getHotkeyINI(int player, int pipe_count, string id)
{
    char buff[256];
    sprintf(buff,
        "[Hotkeys1]\nDevice = Pipe/%d/%s%d\n",
        pipe_count,
        id.c_str(),
        pipe_count);
    string pipeOut(buff);
    pipeOut += hotkey;
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
    printf("CFG: Destroying Config\n");
}
