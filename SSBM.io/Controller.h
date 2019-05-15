#pragma once
#include <string>
#define _NUM_BUTTONS 12

class Controller
{
public:
    static enum Button
    {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,
        Z = 4,
        L = 5,
        R = 6,
        D_UP = 7,
        D_LEFT = 8,
        D_RIGHT = 9,
        D_DOWN = 10,
        START = 11
    };

    Controller();
    ~Controller();

    std::string getState();

    void setButton(Button btn, bool state);
    
    /* setStick
     * bool stick:
     *  true - Main
     *  false - C
     * float valX/Y:
     *  Must be between 0-1. 0.5 is default resting position
     */
    void setStick(bool stick, float valX = 0.5f, float valY = 0.5f);

private:
    // Piped input sticks go from 0 to 1. 0.5 is centered
    float _MainStickX;
    float _MainStickY;
    float _CStickX;
    float _CStickY;

    // A B X Y Z L R D_UP D_LEFT D_RIGHT D_DOWN START
    static const std::string* _btnNames[];
    bool _Buttons[12];
};

