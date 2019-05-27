// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include "Handler.h"

int main()
{
    printf("Creating Handler\n");
    Handler hnd;

    hnd.StartDolphin();

    if (!hnd.IsInitialized())
    {
        //hnd.KillDolphin();
        fprintf(stderr, "Handler not initialized\n");
        exit(EXIT_FAILURE);
    }

    hnd.WaitForDolphinClose();

    printf("Press enter to spam A\n");
    Controller* ctrl = hnd.getController();
    char inp = 'a';
    while ((inp = getchar()) != 'q')
    {
        ctrl->setButton(Button::A);
        ctrl->SendState();
        ctrl->setButton(Button::None);
        ctrl->SendState();
    }

}
