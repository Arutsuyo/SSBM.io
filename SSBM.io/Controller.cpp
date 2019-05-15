#include "Controller.h"
using namespace std;

// The order must match enum Button
const std::string* Controller::_btnNames[] = {
    "A",
    "B",
    "X",
    "Y",
    "Z",
    "L",
    "R",
    "D_UP",
    "D_LEFT",
    "D_RIGHT",
    "D_DOWN",
    "START"
};

Controller::Controller()
{
    _MainStickX = 0.5f;
    _MainStickY = 0.5f;
    _CStickX = 0.5f;
    _CStickY = 0.5f;

    memset(_Buttons, false, sizeof(bool) * _NUM_BUTTONS);
}

string Controller::getState()
{
    char buff[256];
    string output = string();

    // Main Stick
    sprintf(buff, "SET MAIN %.2f %.2f\n", _MainStickX, _MainStickY);
    output += buff;

    // C Stick
    sprintf(buff, "SET C %.2f %.2f\n", _CStickX, _CStickY);
    output += buff;

    // buttons
    for (unsigned int i = 0; i < _NUM_BUTTONS; i++)
    {
        sprintf(buff, "%s %s \n", _Buttons[i] ? "PRESS" : "RELEASE", _btnNames[i]);
        output += buff;
    }
}

void Controller::setButton(Button btn, bool state)
{
    _Buttons[btn] = state;
}

void Controller::setStick(bool stick, float valX, float valY)
{
    if (stick)
    {
        _MainStickX = valX;
        _MainStickY = valY;
    }
    else
    {
        _CStickX = valX;
        _CStickY = valY;
    }
}

Controller::~Controller()
{
}
