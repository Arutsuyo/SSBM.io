#pragma once

#ifndef _HANDLER_
#define _HANDLER_

#include "Config.h"
#include "DolphinHandle.h"
#include <vector>
#include <mutex>
#include <condition_variable>

// Signal handlers for CTRL^C
void sigint_handle(int val);
bool createSigIntAction();

bool exists_test(const std::string& name);
bool dir_exists(const char* path);

class Trainer
{
    std::vector<DolphinHandle*> _Dhandles;
    static std::vector<int> killpids;
public:
    static Trainer* _inst;

    static bool term;
    bool initialized;
    static Config* cfg;

    // ISO info
    static std::string _ssbmisoLocs[];
    static int _isoidx;

    // Directory info
    static std::string userDir;
    static std::string dolphinDefaultUser;
    static std::string PythonCommand;

    // Threading Info
    static unsigned Concurent;
    static VsType vs;
    
    static std::mutex mut;
    static std::condition_variable cv;

    static void AddToKillList(int pid);
    static void KillAllpids();

    void KillDolphinHandles();
    void runTraining();
    Trainer();
    ~Trainer();
};

#endif