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
        printf("CTRL: Bad Signal Catch: %d\n", val);
        return;
    }

    printf("CTRL: A pipe reader was closed\n");
}

bool createSigPipeAction()
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
    if (!pipeOpen)
    {
        fprintf(stderr, "CTRL: Cannot send input, please open pipe");
        return false;
    }

    return sendtofifo(GetState());
}

void Controller::setButton(Button btn)
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

bool Controller::CreateFifo(string inPipePath, int pipe_count)
{
    printf("CTRL: Creating pipe\n");
    // Make the pipe
    pipePath = inPipePath + to_string(pipe_count);
    if(mkfifo(pipePath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == -1)
    {
        perror("CTRL: mkfifo failed");
        return false;
    }
    printf("CTRL: Pipe Created: %s\n", pipePath.c_str());
    return true;
}

bool Controller::OpenController()
{
    printf("CTRL: Creating pipe signal handler\n");
    if (!createSigPipeAction())
        printf("CTRL: Could not create fifo sighandler\n");

    // Blocks if no reader
    printf("CTRL: Opening pipe\n");
    if ((fifo_fd = open(pipePath.c_str(), O_WRONLY)) < 0)
    {
        perror("Could not open fifo");
        pipeOpen = false;
        return false;
    }
    pipeOpen = true;

    printf("CTRL: Controller ready for input!\n");

    return true;
}

bool Controller::IsPipeOpen()
{
    return pipeOpen;
}

Controller::Controller()
{
    printf("CTRL: Initializing Controller\n");
    _MainStickX = 0.5f;
    _MainStickY = 0.5f;

    for (int i = 0; i < _NUM_BUTTONS; i++)
        _Buttons[i] = false;
}

Controller::~Controller()
{
    printf("CTRL: Destroying Controller\n");
    if (pipeOpen)
    {
        printf("CTRL: Closing pipe\n");
        close(fifo_fd);
    }
    printf("CTRL: deleting pipe\n");
    if (remove(pipePath.c_str()) != 0)
        perror("CTRL: Error deleting pipe");
    printf("CTRL: deleted pipe\n");
    
}
