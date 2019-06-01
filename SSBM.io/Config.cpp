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

string Dini =
"[General]\n"
"LastFilename = /mnt/f/Program Files/Dolphin-x64/iso/ssbm.gcm\n"
"ShowLag = False\n"
"ShowFrameCount = False\n"
"ISOPaths = 1\n"
"RecursiveISOPaths = False\n"
"NANDRootPath = \n"
"WirelessMac = \n"
"ISOPath0 = /mnt/f/Program Files/Dolphin-x64/iso\n"
"[Interface]\n"
"ConfirmStop = True\n"
"UsePanicHandlers = True\n"
"OnScreenDisplayMessages = True\n"
"HideCursor = False\n"
"AutoHideCursor = False\n"
"MainWindowPosX = 100\n"
"MainWindowPosY = 100\n"
"MainWindowWidth = 800\n"
"MainWindowHeight = 600\n"
"Language = 0\n"
"ShowToolbar = True\n"
"ShowStatusbar = True\n"
"ShowLogWindow = False\n"
"ShowLogConfigWindow = False\n"
"ExtendedFPSInfo = False\n"
"ThemeName40 = Clean\n"
"PauseOnFocusLost = False\n"
"[Display]\n"
"FullscreenResolution = Auto\n"
"Fullscreen = False\n"
"RenderToMain = False\n"
"RenderWindowXPos = 2190\n"
"RenderWindowYPos = 127\n"
"RenderWindowWidth = 1435\n"
"RenderWindowHeight = 846\n"
"RenderWindowAutoSize = False\n"
"KeepWindowOnTop = False\n"
"ProgressiveScan = False\n"
"PAL60 = True\n"
"DisableScreenSaver = True\n"
"ForceNTSCJ = False\n"
"[GameList]\n"
"ListDrives = False\n"
"ListWad = True\n"
"ListElfDol = True\n"
"ListWii = True\n"
"ListGC = True\n"
"ListJap = True\n"
"ListPal = True\n"
"ListUsa = True\n"
"ListAustralia = True\n"
"ListFrance = True\n"
"ListGermany = True\n"
"ListItaly = True\n"
"ListKorea = True\n"
"ListNetherlands = True\n"
"ListRussia = True\n"
"ListSpain = True\n"
"ListTaiwan = True\n"
"ListWorld = True\n"
"ListUnknown = True\n"
"ListSort = 3\n"
"ListSortSecondary = 0\n"
"ColorCompressed = True\n"
"ColumnPlatform = True\n"
"ColumnBanner = True\n"
"ColumnNotes = True\n"
"ColumnFileName = False\n"
"ColumnID = False\n"
"ColumnRegion = True\n"
"ColumnSize = True\n"
"ColumnState = True\n"
"[Core]\n"
"HLE_BS2 = False\n"
"TimingVariance = 40\n"
"CPUCore = 1\n"
"Fastmem = True\n"
"CPUThread = True\n"
"DSPHLE = True\n"
"SkipIdle = True\n"
"SyncOnSkipIdle = True\n"
"SyncGPU = False\n"
"SyncGpuMaxDistance = 200000\n"
"SyncGpuMinDistance = -200000\n"
"SyncGpuOverclock = 1.00000000\n"
"FPRF = False\n"
"AccurateNaNs = False\n"
"DefaultISO = \n"
"DVDRoot = \n"
"Apploader = \n"
"EnableCheats = False\n"
"SelectedLanguage = 0\n"
"OverrideGCLang = False\n"
"DPL2Decoder = False\n"
"Latency = 2\n"
"MemcardAPath = /home/nara/.local/share/dolphin-emu/GC/MemoryCardA.USA.raw\n"
"MemcardBPath = /home/nara/.local/share/dolphin-emu/GC/MemoryCardB.USA.raw\n"
"AgpCartAPath = \n"
"AgpCartBPath = \n"
"SlotA = 1\n"
"SlotB = 255\n"
"SerialPort1 = 255\n"
"BBA_MAC = \n"
"SIDevice0 = 6\n"
"AdapterRumble0 = True\n"
"SimulateKonga0 = False\n"
"SIDevice1 = 6\n"
"AdapterRumble1 = True\n"
"SimulateKonga1 = False\n"
"SIDevice2 = 0\n"
"AdapterRumble2 = True\n"
"SimulateKonga2 = False\n"
"SIDevice3 = 0\n"
"AdapterRumble3 = True\n"
"SimulateKonga3 = False\n"
"WiiSDCard = False\n"
"WiiKeyboard = False\n"
"WiimoteContinuousScanning = False\n"
"WiimoteEnableSpeaker = False\n"
"RunCompareServer = False\n"
"RunCompareClient = False\n"
"EmulationSpeed = 1.00000000\n"
"FrameSkip = 0x00000000\n"
"Overclock = 1.00000000\n"
"OverclockEnable = False\n"
"GFXBackend = \n"
"GPUDeterminismMode = auto\n"
"PerfMapDir = \n"
"[Movie]\n"
"PauseMovie = False\n"
"Author = \n"
"DumpFrames = False\n"
"DumpFramesSilent = False\n"
"ShowInputDisplay = False\n"
"[DSP]\n"
"EnableJIT = True\n"
"DumpAudio = False\n"
"DumpUCode = False\n"
"Backend = ALSA\n"
"Volume = 100\n"
"CaptureLog = False\n"
"[Input]\n"
"BackgroundInput = True\n"
"[FifoPlayer]\n"
"LoopReplay = True\n"
"[Analytics]\n"
"ID = 7ffb3ccbca9f89e9e1dac93c6041f2d1\n"
"Enabled = False\n"
"PermissionAsked = True\n";


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
