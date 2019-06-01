#include "DolphinHandle.h"
#include "MemoryScanner.h"
#include "Player.h"
#include "addresses.h"
#include "Trainer.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
using namespace std;

#ifdef WIN32
#define SIGKILL 9
int fork() {};
void kill(int pid, int signal) {};
int wait(int* status) {};
#endif

std::string memlocation = 
"004530E0\n"
"00453F70\n"
"0045310E\n"
"00453F9E\n"
"004530C0\n"
"00453F50\n"
"00453090\n"
"00453F20\n"
"00453094\n"
"00453F24\n"
"00453130 70\n"
"00453130 20CC\n"
"00453130 8F4\n"
"00453130 19EC\n"
"00453130 19BC\n"
"00453130 23a0\n"
"00453130 2174\n"
"00453130 19C8\n"
"00453130 140\n"
"00453130 E0\n"
"00453130 E4\n"
"00453130 EC\n"
"00453130 F0\n"
"00453130 14C\n"
"00453FC0 70\n"
"00453FC0 20CC\n"
"00453FC0 8F4\n"
"00453FC0 19EC\n"
"00453FC0 19BC\n"
"00453FC0 23a0\n"
"00453FC0 2174\n"
"00453FC0 19C8\n"
"00453FC0 140\n"
"00453FC0 E0\n"
"00453FC0 E4\n"
"00453FC0 EC\n"
"00453FC0 F0\n"
"00453FC0 14C\n"
"003F0E0A\n"
"003F0E2E\n"
"00479d30\n"
"004D6CAD\n"
"0111826C\n"
"01118270\n"
"00479D60\n"
"\n";


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

void SendKill(int pid)
{
    printf("Sending kill signal\n");
    kill(pid, SIGINT);
}

bool WaitForDolphinToClose(int pid)
{
    int status;
    int tpid;
    SendKill(pid);
    tpid = waitpid(pid, &status, 0);
    printf("Child(Status:%d) Exited\n", status);
    return true;
}

void DolphinHandle::CopyBaseFiles()
{
    char buff[1028];
    sprintf(buff, "cp -r ./Files/* %s", dolphinUser.c_str());
    system(buff);
}

string DolphinHandle::AddController(int player, int pipe_count, string id)
{
    printf("DH: Creating AI Player: %d\n", player);
    Controller* ctrl = new Controller;
    !ctrl->CreateFifo(aiPipe, pipe_count);
    controllers.push_back(ctrl);
    return Trainer::cfg->getAIPipeConfig(player, pipe_count, id);
}

void DolphinHandle::dolphin_thread(ThreadArgs* targ)
{
    printf("DH-T-: --Thread Started\n");
    ThreadArgs ta = *targ;
    *ta._pid = fork();
    // Child
    if (*ta._pid == 0)
    {
        printf("DH-T-: Launching Dolphin\n");
        const char* uLoc = ta._dolphinUser.c_str();
        // Create the Dolphin call
        int lngth = 7;
        char** argv = (char**)malloc(sizeof(char*) * lngth);
        for (int i = 0; i < lngth - 1; i++)
            argv[i] = (char*)malloc(sizeof(char) * 128);
        argv[lngth - 1] = NULL;

        sprintf(argv[0], "dolphin-emu");
        sprintf(argv[1], "-b");
        sprintf(argv[2], "-e");
        sprintf(argv[3], "%s", Trainer::_ssbmisoLocs[Trainer::_isoidx].c_str());
        sprintf(argv[4], "-u");
        sprintf(argv[5], "%s", ta._dolphinUser.c_str());

        execvp(argv[0], argv);

        fprintf(stderr, "DH-T-: EXECVP FAILED!\n");
        exit(EXIT_FAILURE);
    } // child will not exit this block

    // Parent Code

    // Check if Fork Failed
    if (*ta._pid == -1)
    {
        fprintf(stderr, "DH-T-: FORK FAILED\n");
        *ta._running = false;
        return;
    }

    printf("%d:Child successfully launched\n", *ta._pid);

    // Threads can and most likely will intercept ctrl^c
    if (!createSigIntAction())
        printf("%d:Could not create SIGINT handler\n", *ta._pid);

    // Do this AFTER launching Dolphin, otherwise it will block
    printf("%d:Opening controllers\n", *ta._pid);
    for (int i = 0; i < (*ta._controllers).size(); i++)
    {
        if (!(*ta._controllers)[i]->OpenController())
        {
            *ta._running = false;
            return;
        }
    }
    
    MemoryScanner mem = MemoryScanner(ta._dolphinUser);


    // Do Input
    Trainer::cv.notify_all();
    sleep(10);
    bool openPipe = (*ta._controllers).back()->ActivateSaveState();
    printf("%d:Ready for input!\n", *ta._pid);

    bool alt = true;
    while (*ta._running && openPipe)
    {
        for (int i = 0; i < (*ta._controllers).size(); i++)
        {
            (*ta._controllers)[i]->setButton(alt ? Button::A : Button::None);
            openPipe = (*ta._controllers)[i]->SendState();
            mem.UpdatedFrame();
        }
        alt = !alt;
        sleep(1);
    }

    // Closing, notify the trainer
    printf("%d:Closing Thread\n", *ta._pid);
    WaitForDolphinToClose(*ta._pid);
    *ta._running = false;
    Trainer::cv.notify_all();
}

bool DolphinHandle::StartDolphin(int lst)
{
    char buff[256];
    sprintf(buff, "/ssbm%d/", lst);
    dolphinUser = Trainer::userDir + buff;

    // Copy the user folder
    printf("DH: Prepping the user folder: %s\n", dolphinUser.c_str());
    sprintf(buff, "cp -r %s %s", Trainer::dolphinDefaultUser.c_str(), dolphinUser.c_str());
    system(buff);

    CopyBaseFiles();

    running = true;
    int player = 0;
    int pipe_count = 0;
    string id = "ssbm.io";

    // Making sure the pipe folder exists
    aiPipe = dolphinUser + "Pipes/";
    sprintf(buff, "mkdir %s", aiPipe.c_str());
    system(buff);
    aiPipe += id;

    // Make the GCPadNew.ini
    string dolphinConfig = dolphinUser + "Config/";
    controllerINI = "";

    std::string memwatch = dolphinUser + "MemoryWatcher/";
    
    string hotkey;
    switch (_vs)
    {
    case Human:
    case CPU:
        controllerINI += Trainer::cfg->getPlayerPipeConfig(player++);
        hotkey = Trainer::cfg->getHotkeyINI(player, pipe_count, id);
        controllerINI += AddController(player++, pipe_count++, id);
        break;

    case Self:
        controllerINI = AddController(player++, pipe_count++, id);
        hotkey = Trainer::cfg->getHotkeyINI(player, pipe_count, id);
        controllerINI += AddController(player++, pipe_count++, id);
        break;

    default:
        fprintf(stderr, "DH: Invalid VS Type: %d", _vs);
        return false;
        break;
    }

    // Write the pipe ini
    GCPadNew = dolphinConfig + "GCPadNew.ini";
    printf("DH: Writing %s\n", GCPadNew.c_str());
    sprintf(buff, "mkdir %s", dolphinConfig.c_str());
    system(buff);
    FILE* fd = fopen(GCPadNew.c_str(), "w");
    if (!fd)
    {
        char buff[256];
        sprintf(buff, "DH: fopen:%s failed", GCPadNew.c_str());
        perror(buff);
        return false;
    }
    fwrite(controllerINI.c_str(), sizeof(char), controllerINI.size(), fd);
    fclose(fd);

    puts("creating memwatch directory");
    sprintf(buff, "mkdir %s", memwatch.c_str());
    system(buff);
    memwatch += "Locations.txt";

    FILE* fd1 = fopen(memwatch.c_str(), "w");
    if(!fd1){
    	char buff[256];
    	sprintf(buff, "DH: fopen:%s failed", memwatch.c_str());
    	perror(buff);
    	return false;
    }

    fwrite(memlocation.c_str(), sizeof(char), memlocation.size(), fd1);
    
    fclose(fd1);
    
    // Write the hotkey for savestate
    dolphinConfig += "Hotkeys.ini";
    fd = fopen(dolphinConfig.c_str(), "w");
    fwrite(hotkey.c_str(), sizeof(char), hotkey.size(), fd);
    fclose(fd);

    ThreadArgs* ta = new ThreadArgs;
    ta->_running = &running;
    ta->_pid = &pid; 
   ta->_dolphinUser = dolphinUser;
    ta->_controllers = &controllers;

    printf("DH: Starting Thread\n");
    unique_lock<mutex> lk(Trainer::mut);
    t = new thread(&DolphinHandle::dolphin_thread, ta);
    Trainer::cv.wait(lk);
    started = true;
    return true;
}

DolphinHandle::DolphinHandle(VsType vs) :
    t(NULL),
    _vs(vs)
{
    running = false;
    started = false;
}

DolphinHandle::~DolphinHandle()
{
    running = false;
    printf("DH: Closing Dolphin Handle, Closing Thread\n");
    if (t->joinable())
        t->join();
    if (targ)
        delete targ;
    // Call each destructor
    while (controllers.size() > 0)
        controllers.pop_back();

    // Call each destructor
    while (controllers.size() > 0)
    {
        Controller* ctrl = controllers[0];
        ctrl->~Controller();
        controllers.pop_back();
    }

    // Delete the user folder
    char buff[256];
    printf("DH: Deleting the created user folder\n");
    sprintf(buff, "rm -rf %s", dolphinUser.c_str());
    system(buff);
}
