#pragma once
#include <string>
#include <stdio.h>
#define _NUM_BUTTONS 5

class Controller
{
    bool initialized;

    FILE* outPipe;

    // Piped input sticks go from 0 to 1. 0.5 is centered
    float _MainStickX;
    float _MainStickY;

    // A B X Y Z L R D_UP D_LEFT D_RIGHT D_DOWN START
    static const std::string* _btnNames[];
    bool _Buttons[12];

public:
    // The order must match _btnNames[]
    static enum Button
    {
        A = 0,
        B = 1,
        X = 2,
        Z = 3,
        L = 4,
        None
    };

    Controller(const char* pipePath);
    ~Controller();

    // This should only be explicitly called for debug use, 
    // otherwise it's use internally to send the state
    std::string GetState();

    // Primary call the AI will use to send the state to Dolphin.
    void SendState();

    // Sets only the designated button to true, 
    // all other buttons will be set to false
    void setButton(Button btn, bool state);
    
    /* setStick
     * float valX/Y:
     *  Must be between 0-1. 0.5 is default resting position
     */
    void setStick(float valX = 0.5f, float valY = 0.5f);

    bool IsInitialized();
private:
};

