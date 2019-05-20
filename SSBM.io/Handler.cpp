#include "Handler.h"
#define _POSIX_SOURCE
#include <signal.h>
#ifndef WIN32
#include <unistd.h>
#endif
using namespace std;


bool Handler::StartDolphin()
{
    if (!IsInitialized())
        return false;

    pid = fork();

    // Child
    if (pid == 0)
    {
        printf("Child Launched!\n");
        printf("Exitting now!\n");
        exit(EXIT_SUCCESS);
    } // child will not exit this block

    // Parent Code

    // Check if Fork Failed
    if (pid == -1)
    {
        fprintf(stderr, "FORK FAILED\n");
        return false;
    }

    return true;
}

bool Handler::KillDolphin()
{
    kill(pid, SIGKILL);
}

bool Handler::WaitForDolphinClose()
{
    int status;
    int tpid;
    tpid = wait(&status);
    if (tpid != pid)
    {
        fprintf(stderr, "Wrong Child(Status:%d) Exited?!", status);
        return false;
    }

    printf("Child(Status:%D) Exited", status);
    return true;
}

Controller* Handler::getController()
{
    if (ctrl && ctrl->IsInitialized())
        return ctrl;
}

bool Handler::IsInitialized()
{
    if (!cfg || !ctrl)
        return false;
    return cfg->IsInitialized() && ctrl->IsInitialized();
}

Handler::Handler(int numAI, int numCPU, int numHuman)
{
    cfg = new Config(Config::Human,
        "%Dolphin%\Dolphin.exe",
        "%Dolphin%\iso\ssbm.gcm");
    ctrl = new Controller("%Dolphin%\Pipe\1\AI1");
}

Handler::~Handler()
{
    if (cfg)
        cfg->~Config();
    if (ctrl)
        ctrl->~Controller();
}
