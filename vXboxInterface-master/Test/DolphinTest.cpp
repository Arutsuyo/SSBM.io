// Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include <iostream>

#include "..\API\vXboxInterface.h"

#ifndef XBOX
#pragma comment(lib, "vXboxInterface")

#define STICK_MAX (short(32767))

using namespace std;

int main()
{
    BOOL res;
    UCHAR nEmpty;

    int xInputUser = -1;

    // Test if bus exists
    BOOL bus = isVBusExists();
    if (bus)
        printf("Virtual Xbox bus exists\n\n");
    else
    {
        printf("Virtual Xbox bus does NOT exist - Aborting\n\n");
        getchar();
        return -1;
    }

    // Install Virtual Devices
    for (UINT i = 0; i < 6; i++)
    {
        res = GetNumEmptyBusSlots(&nEmpty);
        if (res)
            printf("\n\nNumber of Empty Slots: %d\n",nEmpty);
        else
            printf("\n\nCannot determine Number of Empty Slots");

        res = isControllerExists(i);
        printf("\nisControllerExists(%d): %X\n", i, res);
        res = isControllerOwned(i);
        printf("\nisControllerOwned(%d): %X\n", i, res);
        res = PlugIn(i);
        printf("Plug-in device %d: %X\n", i, res);
        res = isControllerExists(i);
        printf("isControllerExists(%d): %X\n", i, res);
        res = isControllerOwned(i);
        printf("\nisControllerOwned(%d): %X\n", i, res);
        if (res)
        {
            xInputUser = i;
            break;
        }
    }

    if (xInputUser == -1)
    {
        cerr << "Unable to register controller for AI." << endl;
        exit(EXIT_FAILURE);
    }

    printf("\nUse Virtual Controller %d in dolphin\m", xInputUser - 1);

    printf("Press Any Button To Start!\n");
    getchar();

    printf("Testing/Mapping Inputs\n");

    printf("- - - Face Buttons - - - \n");
    printf("Button A\n");
    SetBtnA(xInputUser, TRUE);
    Sleep(100);
    SetBtnA(xInputUser, FALSE);
    Sleep(100);

    printf("Button B\n");
    SetBtnB(xInputUser, TRUE);
    Sleep(100);
    SetBtnB(xInputUser, FALSE);
    Sleep(100);

    printf("Button X\n");
    SetBtnX(xInputUser, TRUE);
    Sleep(100);
    SetBtnX(xInputUser, FALSE);
    Sleep(100);

    printf("Button Y\n");
    SetBtnY(xInputUser, TRUE);
    Sleep(100);
    SetBtnY(xInputUser, FALSE);
    Sleep(100);

    printf("Button RB\n");
    SetBtnRB(xInputUser, TRUE);
    Sleep(100);
    SetBtnRB(xInputUser, FALSE);
    Sleep(100);

    printf("Button Start\n");
    SetBtnStart(xInputUser, TRUE);
    Sleep(100);
    SetBtnStart(xInputUser, FALSE);
    Sleep(100);

    printf("- - - Control Stick - - -\n");
    printf("Up\n");
    SetAxisY(xInputUser, STICK_MAX);
    Sleep(100);
    SetAxisY(xInputUser, 0);
    Sleep(100);

    printf("Down\n");
    SetAxisY(xInputUser, -STICK_MAX);
    Sleep(100);
    SetAxisY(xInputUser, 0);
    Sleep(100);

    printf("Left\n");
    SetAxisX(xInputUser, -STICK_MAX);
    Sleep(100);
    SetAxisX(xInputUser, 0);
    Sleep(100);

    printf("Right\n");
    SetAxisX(xInputUser, STICK_MAX);
    Sleep(100);
    SetAxisX(xInputUser, 0);
    Sleep(100);

    printf("- - - C Stick - - -\n");
    printf("C Up\n");
    SetAxisRy(xInputUser, STICK_MAX);
    Sleep(100);
    SetAxisRy(xInputUser, 0);
    Sleep(100);

    printf("C Down\n");
    SetAxisRy(xInputUser, -STICK_MAX);
    Sleep(100);
    SetAxisRy(xInputUser, 0);
    Sleep(100);

    printf("C Left\n");
    SetAxisRx(xInputUser, -STICK_MAX);
    Sleep(100);
    SetAxisRx(xInputUser, 0);
    Sleep(100);

    printf("C Right\n");
    SetAxisRx(xInputUser, STICK_MAX);
    Sleep(100);
    SetAxisRx(xInputUser, 0);
    Sleep(100);

    printf("- - - Triggers - - - 0 to 255\n");
    printf("Left Trigger\n");
    SetTriggerL(xInputUser, 255);
    Sleep(100);
    SetTriggerL(xInputUser, 0);
    printf("Right Trigger\n");
    Sleep(100);
    SetTriggerR(xInputUser, 255);
    Sleep(100);
    SetTriggerR(xInputUser, 0);
    Sleep(100);


    printf("Analog (Guessing on this one, probably safe to set the same as the triggers)\n");
    printf("Left Trigger\n");
    SetTriggerL(xInputUser, 255);
    Sleep(100);
    SetTriggerL(xInputUser, 0);
    Sleep(100);
    printf("Right Trigger\n");
    SetTriggerR(xInputUser, 255);
    Sleep(100);
    SetTriggerR(xInputUser, 0);
    Sleep(100);

    printf("- - - DPAD - - -\n");
    printf("DPAD UP\n");
    SetDpad(xInputUser, DPAD_UP);
    Sleep(100);
    printf("DPAD DOWN\n");
    SetDpad(xInputUser, DPAD_DOWN);
    Sleep(100);
    printf("DPAD LEFT\n");
    SetDpad(xInputUser, DPAD_LEFT);
    Sleep(100);
    printf("DPAD RIGHT\n");
    SetDpad(xInputUser, DPAD_RIGHT);
    Sleep(100);
    SetDpad(xInputUser, 0);

    printf("\nMapping Completed!\n");
    printf("Press any key to remove devices \n");
    getchar();

    res = UnPlugForce(xInputUser);
    printf("UnPlug (Forced) device %d: %X\n", xInputUser, res);

#if 0
    // UnInstall Virtual Devices
    for (UINT i = 0; i < 6; i++)
    {
        res = UnPlugForce(i);
        printf("UnPlug (Forced) device %d: %X\n", i, res);
    }
#endif // 0

    printf("Press any key to exit \n");
    getchar();

    return 0;

}
#else // XBOX
int main()
{
    printf("NOT Xbox mode\n");
    return 0;
}

#endif // XBOX 