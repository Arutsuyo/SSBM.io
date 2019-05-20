#include "Controller.h"
using namespace std;

// The order must match enum Button
const std::string* Controller::_btnNames[] = {
    "A",
    "B",
    "X",
    "Z",
    "L"
};

Controller::Controller(const char* pipePath)
{
    if ((outPipe = fopen(pipePath, 'w')) == NULL)
    {
        fprintf(stderr, "Could not open pipe: %s", pipePath);
        initialized = false;
        return;
    }

    _MainStickX = 0.5f;
    _MainStickY = 0.5f;

    memset(_Buttons, false, sizeof(bool) * _NUM_BUTTONS);
}

string Controller::GetState()
{
    char buff[256];
    string output = string();

    // Main Stick
    sprintf(buff, "SET MAIN %.2f %.2f\n", _MainStickX, _MainStickY);
    output += buff;

    // buttons
    for (unsigned int i = 0; i < _NUM_BUTTONS; i++)
    {
        sprintf(buff, "%s %s \n", _Buttons[i] ? "PRESS" : "RELEASE", _btnNames[i]);
        output += buff;
    }

    return output;
}

void Controller::SendState()
{
    fprintf(outPipe, GetState().c_str());
}

void Controller::setButton(Button btn, bool state)
{
    for (int i = 0; i < _NUM_BUTTONS; i++)
        _Buttons[btn] = i == btn ? state : false;
}

void Controller::setStick(float valX, float valY)
{
    _MainStickX = valX;
    _MainStickY = valY;
}

bool Controller::IsInitialized()
{
    return initialized;
}

Controller::~Controller()
{
    if (outPipe)
        fclose(outPipe);
}
