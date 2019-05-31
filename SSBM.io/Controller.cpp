#include "Controller.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <cerrno>
#include <signal.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

char Controller::_ButtonNames[] = {
        'A',
        'B',
        'X',
        'Z',
        'L'
};

bool Controller::sendtofifo(string fifocmd)
{
    printf("Writing: %s", fifocmd.c_str());
    unsigned int buff_sz = strlen(fifocmd.c_str());
    if (write(fifo_fd, fifocmd.c_str(), buff_sz) < buff_sz)
    {
        perror("Error writing to fifo");
        pipeOpen = false;
        return false;
    }
    return true;
}

void sigpipe_handler(int val)
{
    if (val != SIGPIPE)
    {
        printf("Bad Signal Catch: %d\n", val);
        return;
    }

    printf("A pipe reader was closed\n");
}

bool Controller::createSigAction()
{
    // Create signal action
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sigpipe_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGPIPE, &sa, NULL) == -1)
    {
        fprintf(stderr, "sigaction Failed\n");
        return true;
    }
    return false;
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
            _ButtonNames[i]);
        output += buff;
    }

    return output;
}

bool Controller::SendState()
{
    if (!initialized || !pipeOpen)
    {
        fprintf(stderr, "Cannot send input, please open pipe");
        return false;
    }

    return sendtofifo(GetState());
}

void Controller::setButton(Button btn = Button::None)
{
    for (int i = 0; i < _NUM_BUTTONS; i++)
        _Buttons[i] = i == btn ? true : false;

}

void Controller::setSticks(float valX, float valY)
{
    _MainStickX = valX;
    _MainStickY = valY;
}

string Controller::GetControllerPath()
{
    return pipePath;
}

bool Controller::CreateFifo(string& inPipePath, int &pipe_count)
{
    printf("Creating pipe\n");
    // Make the pipe
    pipePath = inPipePath;
    pipe_count = 0;
    while (mkfifo(inPipePath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == -1)
    {
        perror("mkfifo failed");
        pipe_count++;
        inPipePath = pipePath + to_string(pipe_count);
    }
    // Save the final pipe path
    pipePath = inPipePath;
    printf("Pipe Created: %s\n", pipePath.c_str());
}

bool Controller::SetControllerPath(string &inPipePath)
{
    // Save the pipe path
    pipePath = inPipePath;
    printf("Creating pipe signal handler\n");
    if (createSigAction())
        perror("Could not create fifo handler");

    // Blocks if no reader
    printf("Opening pipe\n");
    if ((fifo_fd = open(pipePath.c_str(), O_WRONLY)) < 0)
    {
        perror("Could not open fifo");
        initialized = false;
        return false;
    }
    pipeOpen = true;

    printf("Controller ready for input!\n");

    initialized = true;
    return true;
}

bool Controller::IsInitialized()
{
    return initialized && pipeOpen;
}

Controller::Controller()
{
    _MainStickX = 0.5f;
    _MainStickY = 0.5f;

    for (int i = 0; i < _NUM_BUTTONS; i++)
        _Buttons[i] = false;
}

Controller::~Controller()
{
    printf("Destroying Controller\n");
    if (pipeOpen)
    {
        printf("Closing pipe\n");
        close(fifo_fd);
    }
    printf("deleting pipe\n");
    if (remove(pipePath.c_str()) != 0)
        perror("Error deleting pipe");
    printf("deleted pipe\n");
    
}
