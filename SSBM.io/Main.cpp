// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <unistd.h>
#include "Trainer.h"

// Included for PW
#include <pwd.h>


int main()
{
    printf("MAIN: Initializing statics\n");
    // Init the static user dir
    struct passwd* pw = getpwuid(getuid());
    Trainer::userDir = pw->pw_dir;
    Trainer::dolphinDefaultUser = Trainer::userDir + "/.local/share/dolphin-emu/";
    Trainer::concurentThreadsSupported = std::thread::hardware_concurrency();
    Trainer::term = false;
    printf("MAIN: Creating Trainer\n");
    Trainer trainer(VsType::Human);
    if (!trainer.initialized)
        exit(EXIT_FAILURE);

    printf("MAIN: Running Training Loop\n");
    trainer.runTraining();

    printf("MAIN: Closing Everything\n");
    trainer.~Trainer();

    printf("MAIN: Shutting Down\n\n");
    exit(EXIT_SUCCESS);
}
