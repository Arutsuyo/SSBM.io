#include "Controller.h"
#include <sys/types.h>
#include <sys/stat.h>
using namespace std;

Controller::Controller()
{
    _MainStickX = 0.5f;
    _MainStickY = 0.5f;

    for (int i = 0; i < _NUM_BUTTONS; i++)
        _Buttons[i] = false;
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
            "%s %c \n",
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

    fwrite(GetState().c_str(), sizeof(char), GetState().size(), outPipe);
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

bool Controller::SetControllerPath(const char* pipePath)
{
    if (mkfifo(pipePath, 0777))
    {
        fprintf(stderr, "Could not create pipe: %s", pipePath);
        initialized = false;
        return false;
    }
    if ((outPipe = fopen(pipePath, "w")) == NULL)
    {
        fprintf(stderr, "Could not open pipe: %s", pipePath);
        initialized = false;
        return false;
    }
    
    initialized = true;
    return true;
}
