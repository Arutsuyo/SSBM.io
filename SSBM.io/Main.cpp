// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include "Handler.h"

int main()
{
    printf("Creating Handler\n");
    Handler hnd();
    bool initialized = hnd.IsInitialized();

    if (!initialized)
    {
        fprintf(stderr, "Handler not initialized\n");
        exit(EXIT_FAILURE);
    }

    hnd.StartDolphin();
    hnd.WaitForDolphinClose();

    Controller* ctrl = hnd.getController();

    printf("Press enter to spam A\n");
    char inp = 'a';
    while ((inp = getchar()) != 'q')
    {
        ctrl->setButton(Button::A);
        ctrl->SendState();
        ctrl->setButton(Button::None);
        ctrl->SendState();
    }

}
