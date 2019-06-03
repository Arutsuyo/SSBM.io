#include "Trainer.h"
#include <fstream>
#include <signal.h>
#include <stdio.h>
#include <cstring>
#define FILENM "TRNR"

bool Trainer::term;

Config* Trainer::cfg;

// ISO locations, will step through
std::string Trainer::_ssbmisoLocs[] = {
    "/mnt/f/Program Files/Dolphin-x64/iso/ssbm.gcm",
    "/mnt/c/Program Files/Dolphin-x64/iso/ssbm.gcm",
    "/mnt/c/User/Nara/Desktop/Dolphin-x64/iso/ssbm.gcm",
};
int Trainer::_isoidx = -1;

// To be filled out in main
std::string Trainer::userDir = "";
std::string Trainer::dolphinDefaultUser = "";

// Used for tracking events in the threads
std::mutex Trainer::mut;
std::condition_variable Trainer::cv;
unsigned Trainer::concurentThreadsSupported;

/* Helper Functions */
inline bool exists_test(const std::string& name) {
    if (FILE * file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

void sigint_handle(int val)
{
    if (val != SIGINT)
        return;

    printf("%s:%d\tReceived SIGINT, closing trainer\n", FILENM, __LINE__);
    Trainer::term = true;
}

bool createSigIntAction()
{
    // Create signal action
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sigint_handle;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "--ERROR:sigaction", strerror(errno));
        return false;
    }

    printf("%s:%d\tSignal Handler Created\n", FILENM, __LINE__);
    return true;
}

void Trainer::runTraining()
{
    printf("%s:%d\tInitializing Training.\n", FILENM, __LINE__);

    createSigIntAction();

    switch (_vs)
    {
    case Self:
        printf("%s:%d\tPlaying vs Self\n", FILENM, __LINE__);
        break;
    case CPU:
        printf("%s:%d\tPlaying vs CPU\n", FILENM, __LINE__);
        break;
    case Human:
        printf("%s:%d\tPlaying vs Human\n", FILENM, __LINE__);
        break;
    default:
        break;
    }

    int numCreate = _vs == VsType::Human ? 1 : concurentThreadsSupported;
    //int numCreate = 1;
    printf("%s:%d\tRunning %d Instance%s\n", FILENM, __LINE__, numCreate, numCreate > 1 ? "s" : "" );
    for (int i = 0; i < numCreate; i++)
    {
        printf("%s:%d\tCreating Handler %d\n", FILENM, __LINE__, i);
        DolphinHandle *dh = new DolphinHandle(_vs);
        _Dhandles.push_back(dh);
    }

    printf("%s:%d\tEntering Management Loop\n", FILENM, __LINE__);
    printf("%s:%d\t--Stop the Trainer with CTRL+C\n", FILENM, __LINE__);
    while (!term)
    {
        std::unique_lock<std::mutex> lk(mut);
        for (int i = 0; i < numCreate; i++)
        {
            DolphinHandle* dh = _Dhandles[i];
            if (!dh->started)
            {
                lk.unlock();
                printf("%s:%d\tStarting(0) Dolphin Instance %d\n", FILENM, __LINE__, i);
                if (!dh->StartDolphin(i))
                {
                    printf("%s:%d\t--ERROR: Dolphin Failed to start(0)\n", FILENM, __LINE__);
                    term = true;
                    break;
                }
                printf("%s:%d\tDolphin Instance %d Started(0)\n", FILENM, __LINE__, i);
                lk.lock();
            }
        }

        if (term)
            break;

        printf("%s:%d\tChecking if running\n", FILENM, __LINE__);
        for (int i = 0; i < numCreate; i++)
        {
            DolphinHandle* dh = _Dhandles[i];
            // Check if the match finished
            if (!dh->running && dh->started)
            {
                printf("%s:%d\tDolphin Instance %d stopped, restarting\n", FILENM, __LINE__, i);
                dh->~DolphinHandle();
                // Remove the handler, calling the destructor
                _Dhandles.erase(_Dhandles.begin() + i);

                // push a new one
                DolphinHandle* dh = new DolphinHandle(_vs);
                lk.unlock();
                printf("%s:%d\tStarting(1) Dolphin Instance %d\n", FILENM, __LINE__, i);
                if (!dh->StartDolphin(i))
                {
                    fprintf(stderr, "%s:%d\t--ERROR:Dolphin Failed to start(1)", FILENM, __LINE__);
                    term = true;
                    break;
                }
                printf("%s:%d\tDolphin Instance %d Started(1)\n", FILENM, __LINE__, i);
                _Dhandles.push_back(dh);
                lk.lock();
            }
        }

        if (term)
            break;

        printf("%s:%d\tWaiting for notification\n", FILENM, __LINE__);
        // Lock the mutex and wait for the condition variable
        cv.wait(lk);
    }
}

Trainer::Trainer(VsType vs)
{
    size_t n = sizeof(_ssbmisoLocs) / sizeof(_ssbmisoLocs[0]);

    do {
        _isoidx++;
        if (_isoidx > n)
        {
            fprintf(stderr, "%s:%d: %s\n", FILENM, __LINE__,
                "--ERROR:ISO not found");
            initialized = false;
            return;
        }
        printf("%s:%d\tTesting for ISO:\n\t%s\n", FILENM, __LINE__, _ssbmisoLocs[_isoidx].c_str());
    } while (!exists_test(_ssbmisoLocs[_isoidx]));


    _vs = vs;
    if (!cfg)
    {
        printf("%s:%d\tCreating Config\n", FILENM, __LINE__);
        cfg = new Config(_vs);
    }

    initialized = true;
}

Trainer::~Trainer()
{
    printf("%s:%d\tTrainer dying\n", FILENM, __LINE__);
    if (cfg)
        cfg->~Config();

    // Call each destructor
    while (_Dhandles.size() > 0)
    {
        DolphinHandle* dh = _Dhandles[0];
        dh->~DolphinHandle();
        _Dhandles.pop_back();
    }
}
