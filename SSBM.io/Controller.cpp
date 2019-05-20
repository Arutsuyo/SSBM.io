#include "Controller.h"
using namespace std;

Controller::Controller()
{
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
        sprintf(
            buff, 
            "%s %s \n", 
            _Buttons[i] ? "PRESS" : "RELEASE", 
            ButtonNames[i]);
        output += buff;
    }

    return output;
}

void Controller::SendState()
{
    if (!initialized)
    {
        fprintf(stderr, "Cannot send input, please set path");
        return;
    }

    fprintf(outPipe, GetState().c_str());
}

void Controller::setButton(Button btn = Button::None)
{
    for (int i = 0; i < _NUM_BUTTONS; i++)
        _Buttons[btn] = i == btn ? true : false;
}

void Controller::setSticks(float valX, float valY)
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

void Controller::SetControllerPath(const char* pipePath)
{
    if ((outPipe = fopen(pipePath, 'w')) == NULL)
    {
        fprintf(stderr, "Could not open pipe: %s", pipePath);
        initialized = false;
        return;
    }
    initialized = true;
}
