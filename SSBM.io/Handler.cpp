#include "Handler.h"
#include <signal.h>
#include <fstream>
#ifndef WIN32
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
// Included for PW
#include <pwd.h>
#else
#define SIGKILL 9
int fork() {};
void kill(int pid, int signal) {};
int wait(int* status) {};
#endif

using namespace std;

// ISO locations, will step through
string Handler::_ssbmisoLocs[] = { 
    "/mnt/f/Program Files/Dolphin-x64/iso/ssbm.gcm",
    "/mnt/c/Program Files/Dolphin-x64/iso/ssbm.gcm",
    "/mnt/c/User/Nara/Desktop/Dolphin-x64/iso/ssbm.gcm",
};

int Handler::_isoidx = -1;

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

string getFileName(const string& s) {

    char sep = '/';

#ifdef _WIN32
    sep = '\\';
#endif

    size_t i = s.rfind(sep, s.length());
    if (i != string::npos) {
        return(s.substr(i + 1, s.length() - i));
    }

    return("");
}

string Handler::getPipePath(string id)
{
    char buff[256];
    sprintf(buff, "Pipes/%s", id.c_str());
    string pipeOut(buff);
    return pipeOut;
}

bool Handler::StartDolphin()
{
    if (!cfg)
        return false;
    if (!cfg->IsInitialized())
        return false;

    int player = 0;
    int pipe_count = 0;
    string id = "ssbm.io";

    string aiPipe = dolphinShared + getPipePath(id);
    string GCPadNew = dolphinUser + "GCPadNew.ini";
    string controllerINI = cfg->getPlayerPipeConfig(player++);

    ctrl->CreateFifo(aiPipe, pipe_count);
    id = getFileName(aiPipe);
    printf("Pipe Name: %s\n", id.c_str());
    controllerINI += cfg->getAIPipeConfig(player++, pipe_count, id);

    printf("Writing GCPadNew.ini\n");
    // Write the pipe ini
    FILE* fd = fopen(GCPadNew.c_str(), "w");
    if (!fd)
    {
        perror("fopen:ini failed");
        exit(EXIT_FAILURE);
    }
    fwrite(controllerINI.c_str(), sizeof(char), controllerINI.size(), fd);
    fclose(fd);

    pid = fork();
    // Child
    if (pid == 0)
    {
        printf("Child: Launched! (TODO exec dolphin)\n");
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

    // Do this AFTER launching Dolphin, otherwise it will block
    printf("Opening controller at: %s\n", aiPipe.c_str());
    ctrl->SetControllerPath(aiPipe);

    printf("Checking Initialization\n");
    if (!IsInitialized())
    {
        fprintf(stderr, "Cannot start, Initialization failed\n");
        return false;
    }

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

Handler::Handler(VsType vs)
{
    struct passwd* pw = getpwuid(getuid());
    pid = -1;
    string userDir = pw->pw_dir;

    dolphinShared = userDir + "/.local/share/dolphin-emu/";
    dolphinUser = userDir + "/.config/dolphin-emu/";

    size_t n = sizeof(_ssbmisoLocs) / sizeof(_ssbmisoLocs[0]);

    do {
        _isoidx++;
        if (_isoidx > n)
        {
            fprintf(stderr, "ERROR: ISO not found\n");
            initialized = false;
            return;
        }
        printf("Testing for ISO:\n%s\n", _ssbmisoLocs[_isoidx].c_str());
    } while (!exists_test(_ssbmisoLocs[_isoidx]));


    printf("Creating Config\n");
    cfg = new Config(vs);
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
