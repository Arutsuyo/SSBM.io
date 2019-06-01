#include "Trainer.h"
#include <fstream>
#include <signal.h>

using namespace std;

bool Trainer::term;

Config* Trainer::cfg;

// ISO locations, will step through
string Trainer::_ssbmisoLocs[] = {
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
inline bool exists_test(const string& name) {
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

    printf("Received SIGINT, closing trainer\n");
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
        fprintf(stderr, "sigaction Failed\n");
        return true;
    }
    return false;
}

void Trainer::runTraining()
{
    printf("TRNR: Initializing Training.\n"
        "TRNR: Press ctrl+c at any time to shut down training\n");

    if (!createSigIntAction())
        printf("TRNR: Could not create SIGINT handler\n");
    
    switch (_vs)
    {
    case Self:
        printf("TRNR: Playing vs Self\n");
        break;
    case CPU:
        printf("TRNR: Playing vs CPU\n");
        break;
    case Human:
        printf("TRNR: Playing vs Human\n");
        break;
    default:
        break;
    }

    int numCreate = _vs == VsType::Human ? 1 : concurentThreadsSupported;
    printf("TRNR: Running %d Instance%s\n", numCreate, numCreate > 1 ? "s" : "" );
    for (int i = 0; i < numCreate; i++)
    {
        printf("TRNR: Creating Handler %d\n", i);
        DolphinHandle *dh = new DolphinHandle(_vs);
        _Dhandles.push_back(dh);
    }

    printf("TRNR: Entering Management Loop\n");

    while (!term)
    {
        unique_lock<mutex> lk(mut);
        for (int i = 0; i < numCreate; i++)
        {
            DolphinHandle* dh = _Dhandles[i];
            if (!dh->started)
            {
                lk.unlock();
                printf("TRNR: Starting(0) Dolphin Instance %d\n", i);
                if (!dh->StartDolphin(i))
                {
                    printf("TRNR: --ERROR: Dolphin Failed to start(0)\n");
                    term = true;
                    break;
                }
                printf("TRNR: Dolphin Instance %d Started(0)\n", i);
                lk.lock();
            }
        }

        if (term)
            break;

        printf("TRNR: Checking if running\n");
        for (int i = 0; i < numCreate; i++)
        {
            DolphinHandle* dh = _Dhandles[i];
            // Check if the match finished
            if (!dh->running && dh->started)
            {
                printf("TRNR: Dolphin Instance %d stopped, restarting\n", i);
                dh->~DolphinHandle();
                // Remove the handler, calling the destructor
                _Dhandles.erase(_Dhandles.begin() + i);

                // push a new one
                DolphinHandle* dh = new DolphinHandle(_vs);
                lk.unlock();
                printf("TRNR: Starting(1) Dolphin Instance %d\n", i);
                if (!dh->StartDolphin(i))
                {
                    printf("TRNR: --ERROR: Dolphin Failed to start(1)");
                    term = true;
                    break;
                }
                printf("TRNR: Dolphin Instance %d Started(1)\n", i);
                _Dhandles.push_back(dh);
                lk.lock();
            }
        }

        if (term)
            break;

        printf("TRNR: Waiting for notification\n");
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
            fprintf(stderr, "TRNR: ERROR: ISO not found\n");
            initialized = false;
            return;
        }
        printf("TRNR: Testing for ISO:\n\t%s\n", _ssbmisoLocs[_isoidx].c_str());
    } while (!exists_test(_ssbmisoLocs[_isoidx]));


    _vs = vs;
    if (!cfg)
    {
        printf("TRNR: Creating Config\n");
        cfg = new Config(_vs);
    }

    initialized = true;
}

Trainer::~Trainer()
{
    printf("TRNR: Trainer dying\n");
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
