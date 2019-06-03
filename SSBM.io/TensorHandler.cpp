#include "TensorHandler.h"
#include <stdio.h>
#include <sys/types.h> /* pid_t */

#ifdef WIN32
// Function definitions here are just because the 
// includes do not exist on windows, which is the platform 
// developed on.
size_t read(int fildes, void* buf, size_t nbytes) {};
size_t write(int fd, const void* buf, size_t count) {};
int pipe(int pipefd[2]) {};
typedef int pid_t;
pid_t fork(void) {};
int Close(int fd) {};
int Redirect(int fd, int fd1) {};
int execlp(const char* file, const char* arg, ...) {};
pid_t waitpid(pid_t pid, int* status, int options) {};
#define STDOUT_FILENO 0
#define STDIN_FILENO 1
#else
#include <unistd.h>
#endif
using namespace std;

bool TensorHandler::CreatePipes()
{
    if (pipe(pipeFromPy) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
            "Pipe", strerror(errno));
        return false;
    }

    if (pipe(pipeToPy) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
            "Pipe", strerror(errno));
        return false;
    }

    if ((pid = fork()) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
            "Fork", strerror(errno));
        return false;
    }

    else if (pid == 0) {
        // Child Process
        Close(pipeFromPy[0]);
        Close(pipeToPy[1]);

        /* redirect stdin and stdout */
        Redirect(pipeFromPy[1], STDOUT_FILENO);
        Redirect(pipeToPy[0], STDIN_FILENO);

        /* run Program1 with redirected stdout */
        execlp("python", "python", "Model/trainer.py");
        
        fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
            "EXECLP", strerror(errno));
    }

    Close(pipeFromPy[1]); /* close unused write end of the pipe */
    Close(pipeToPy[0]); /* close unused read end of the pipe */

    // Send initialization
    char buff[8];
    sprintf(buff, "0");
    int bytesWritten = sizeof(char) * strlen(buff);
    if (write(pipeToPy[0], buff, bytesWritten) != bytesWritten)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
            "write", strerror(errno));
    }

    sprintf(buff, "1");
    bytesWritten = sizeof(char) * strlen(buff);
    if (write(pipeToPy[0], buff, bytesWritten) != bytesWritten)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
            "write", strerror(errno));
    }
}

void TensorHandler::SendToPipe(Player& ai, Player& enemy)
{
    // Send input
    char buff[256];
    sprintf(buff, "%u %d %f %f %u %d %f %f",
        ai.health, ai.dir, ai.pos_x, ai.pos_y,
        enemy.health, enemy.dir, enemy.pos_x, enemy.pos_y);
    int bytesWritten = sizeof(char) * strlen(buff);
    if (write(pipeToPy[1], buff, bytesWritten) != bytesWritten)
        fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
            "write", strerror(errno));
}

std::string TensorHandler::ReadFromPipe()
{
    // Get output
    char buff[1024];
    bool hadRead = false;
    unsigned int ret = 0;
    string output;
    do {
        ret = read(pipeFromPy[0], buff, 1023);
        if (ret == -1)
        {
            fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
                "read", strerror(errno));
            return "";
        }
        else
            hadRead = true;
        buff[1023] = '\0'; // Makes sure it's terminated
        output += buff;
    } while (ret != 0 || !hadRead);

    printf("TH: Tensor output:\n\t%s\n", output.c_str());
    return output;
}

std::string TensorHandler::MakeExchange(Player ai, Player enemy)
{
    SendToPipe(ai, enemy);

    return ReadFromPipe();
}

TensorHandler::TensorHandler() :
    pid(-1),
    pipeToPy{ -1,-1 },
    pipeFromPy{ -1,-1 }
{
}

TensorHandler::~TensorHandler()
{
    // Write closing line to Python
    char buff[256];
    sprintf(buff, "-1 -1");
    write(pipeToPy[0], buff, sizeof(char) * strlen(buff));

    // read from pipe
    bool hadRead = false;
    int ret;
    string output;
    do {
        ret = read(pipeFromPy[0], buff, 256);
        if (ret == -1)
        {
            fprintf(stderr, "%s:%d: %s: %s\n", "TH", __LINE__,
                "read", strerror(errno));
        }
        else
            hadRead = true;
        output += buff;
    } while (ret != 0 || !hadRead);

    // Make sure we can find the closing identifier
    if (output.find("-1 -1") == string::npos)
    {
        fprintf(stderr, "%s:%d: %s\n", "TH", __LINE__, 
            "Tensor Did not close properly");
    }

    // Close the pipes
    Close(pipeFromPy[0]);
    Close(pipeToPy[1]);

    // Wait for Tensor to close
    int status;
    int tpid;
    tpid = waitpid(pid, &status, 0);
    printf("TH: Tensor:%d Exited\n", status);
}
