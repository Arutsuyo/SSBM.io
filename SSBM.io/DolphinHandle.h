#pragma once

#include "Controller.h"
#include <thread>
#include <string>
#include <vector>

struct ThreadArgs
{
    bool* _running;
    bool* _pipes;
    bool* _safeClose;
    int *_pid;
    std::string _dolphinUser;
    std::vector<Controller*> *_controllers;
};

class DolphinHandle
{
    std::thread *t;
    ThreadArgs* targ;
    int pid;
    std::vector<Controller*> controllers;
    VsType _vs = VsType::Self;

    // Important Locations
    std::string dolphinUser;
    std::string aiPipe;

    void CopyBaseFiles();
    std::string AddController(int player, int pipe_count, std::string id);

    static bool dolphin_thread(ThreadArgs *ta);

    static bool CheckClose(ThreadArgs& ta, bool force = false);

public:
    bool running;
    bool started;
    bool safeclose = false;

    bool StartDolphin(int lst);

    DolphinHandle(VsType _vs);
    ~DolphinHandle();
};

