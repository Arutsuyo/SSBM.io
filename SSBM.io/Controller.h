#pragma once
#ifndef _CONTROLLER_
#define _CONTROLLER_

#include <string>
#include <stdio.h>
#include "Types.h"

class Controller
{
    // The order must match enum Button
    static char _ButtonNames[];

    bool initialized = false;

    FILE* outPipe;

    // Piped input sticks go from 0 to 1. 0.5 is centered
    float _MainStickX;
    float _MainStickY;

    bool _Buttons[_NUM_BUTTONS];

public:

    Controller();
    ~Controller();

    bool SetControllerPath(const char* pipePath);

    // This should only be explicitly called for debug use, 
    // otherwise it's use internally to send the state
    std::string GetState();

    // Primary call the AI will use to send the state to Dolphin.
    void SendState();

    // Sets only the designated button to true, 
    // all other buttons will be set to false
    void setButton(Button btn);
    
    /* setStick
     * float valX/Y:
     *  Must be between 0-1. 0.5 is default resting position
     */
    void setSticks(float valX = 0.5f, float valY = 0.5f);

    bool IsInitialized();
private:
};


#endif