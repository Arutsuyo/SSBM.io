// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <unistd.h>
#include "Handler.h"

int main()
{
    printf("Creating Handler\n");
    Handler hnd(VsType::Human);

    hnd.StartDolphin();

    if (!hnd.IsInitialized())
    {
        //hnd.KillDolphin();
        fprintf(stderr, "Handler not initialized\n");
        exit(EXIT_FAILURE);
    }

    hnd.WaitForDolphinClose();

    printf("Looping Input!\n");
    Controller* ctrl = hnd.getController();
    
    while (ctrl->IsInitialized())
    {
        ctrl->setButton(Button::A);
        ctrl->SendState();
        sleep(1);
        ctrl->setButton(Button::None);
        ctrl->SendState();
        sleep(5);
    }

    printf("Closing\n");
}
