#include "Handler.h"
#include <signal.h>
#include <fstream>
#ifndef WIN32
#include <sys/wait.h>
#include <unistd.h>
#else
#define SIGKILL 9
int fork() {};
void kill(int pid, int signal) {};
int wait(int* status) {};
#endif

using namespace std;

string Handler::dolphinexe = "/mnt/f/Program Files/Dolphin-x64/";
string Handler::_dolphinLoc = "/mnt/f/Program Files/Dolphin-x64/Dolphin.exe";
string Handler::_ssbmisoLoc = "/mnt/f/Program Files/Dolphin-x64/iso/ssbm.gcm";
string Handler::dolphinuser = "/mnt/f/Nara/OneDrive/Documents/Dolphin Emulator/";
string Handler::_customINI = "/mnt/f/Nara/OneDrive/Documents/Dolphin Emulator/Config/";

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

void copyFile(const char* src, const char* dst)
{
    printf("Copying %s to %s\n", src, dst);
    ifstream  op(src, ios::binary);
    ofstream  cp(dst, ios::binary);

    cp << op.rdbuf();
}

bool Handler::StartDolphin()
{
    ctrl->SetControllerPath((dolphinuser + "Pipe/AI1").c_str());

    printf("Checking Initialization\n");
    if (!IsInitialized())
    {
        fprintf(stderr, "Cannot start, Initialization failed\n");
        return false;
    }

    printf("Copying Dolphin.ini\n");
    string dolphinCFG = cfg->getConfig();
    string configLoc = _customINI + "Dolphin.ini";
    copyFile(
        configLoc.c_str(),
        (configLoc + ".bkp").c_str());
    FILE * ini = fopen(configLoc.c_str(), "w");
    fwrite(dolphinCFG.c_str(), 
        dolphinCFG.size() * sizeof(char), 
        dolphinCFG.size(), ini);
    fclose(ini);

    printf("Copying Pad.ini\n");
    string padCFG = cfg->getPipeConfig(1);
    string padLoc = _customINI + "GCPadNew.ini";
    copyFile(
        padLoc.c_str(),
        (padLoc + ".bkp").c_str());
    ini = fopen(padLoc.c_str(), "w");
    fwrite(padCFG.c_str(), padCFG.size() * sizeof(char), padCFG.size(), ini);
    fclose(ini);

    pid = fork();
    // Child
    if (pid == 0)
    {
        printf("Child: Launched!\n");
        printf("Exiting now!\n");
        exit(EXIT_SUCCESS);
    } // child will not exit this block

    // Parent Code

    // Check if Fork Failed
    if (pid == -1)
    {
        fprintf(stderr, "FORK FAILED\n");
        return false;
    }

    printf("Child successfully launched\n");
    return true;
}

void Handler::KillDolphin()
{
    printf("Sending kill signal\n");
    kill(pid, SIGKILL);
}

bool Handler::WaitForDolphinClose()
{
    int status;
    int tpid;
    tpid = wait(&status);
    if (tpid != pid)
    {
        fprintf(stderr, "Wrong Child(Status:%d) Exited?!\n", status);
        return false;
    }
    printf("Child(Status:%d) Exited\n", status);

    return true;
}

Controller* Handler::getController()
{
    if (ctrl && ctrl->IsInitialized())
        return ctrl;
    return NULL;
}

bool Handler::IsInitialized()
{
    if (!cfg || !ctrl)
    {
        fprintf(stderr, "Subclass failed to construct");
        return false;
    }
    return cfg->IsInitialized() && ctrl->IsInitialized();
}

Handler::Handler()
{
    pid = -1;

    // Check for existing paths
    if (!exists_test(_dolphinLoc))
    {
        fprintf(stderr,
            "Error: Path to Dolphin exec does not exist:\n%s\n",
            _dolphinLoc.c_str());
        initialized = false;
        return;
    }
    else
        _dolphinLoc = _dolphinLoc;

    if (!exists_test(_ssbmisoLoc))
    {
        fprintf(stderr,
            "Error: Path to ssbm iso does not exist:\n%s\n",
            _ssbmisoLoc.c_str());
        initialized = false;
        return;
    }
    else
        _ssbmisoLoc = _ssbmisoLoc;


    printf("Creating Config\n");
    cfg = new Config(VsType::Human);
    printf("Creating Controller\n");
    ctrl = new Controller();

    initialized = IsInitialized();
}

Handler::~Handler()
{
    if (cfg)
        cfg->~Config();
    if (ctrl)
        ctrl->~Controller();
}
