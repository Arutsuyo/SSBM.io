#include "TensorHandler.h"
#include <sys/types.h> /* pid_t */
#include <cstring>
#include <sys/wait.h>
#include <stdio.h>

#ifdef WIN32
// Function definitions here are just because the 
// includes do not exist on windows, which is the platform 
// developed on.
size_t read(int fildes, void* buf, size_t nbytes) {};
size_t write(int fd, const void* buf, size_t count) {};
int pipe(int pipefd[2]) {};
typedef int pid_t;
pid_t fork(void) {};
int close(int fd) {};
int dup2(int fd, int fd1) {};
int execlp(const char* file, const char* arg, ...) {};
pid_t waitpid(pid_t pid, int* status, int options) {};
#define STDOUT_FILENO 0
#define STDIN_FILENO 1
#else
#include <unistd.h>
#endif

#define FILENM "TH"
using namespace std;

void sigChild_handler(int val)
{
    if (val != SIGCHLD)
    {
        fprintf(stderr, "%s:%d: %s\n", FILENM, __LINE__,
            "Wrong Signal Caught");
        return;
    }

    printf("%s:%d A pipe was closed\n", FILENM, __LINE__);
}

bool createSigChildAction()
{
    // Create signal action
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sigpipe_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGPIPE, &sa, NULL) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "sigaction", strerror(errno));
        return false;
    }
    return true;
}

bool TensorHandler::CreatePipes(Controller* ai)
{
    printf("%s:%d Creating Read Pipe\n", FILENM, __LINE__);

    if (pipe(pipeFromPy) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "Pipe", strerror(errno));
        return false;
    }

    printf("%s:%d Creating Write Pipe\n", FILENM, __LINE__);
    if (pipe(pipeToPy) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "Pipe", strerror(errno));
        return false;
    }

    if ((pid = fork()) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "Fork", strerror(errno));
        return false;
    }

    else if (pid == 0) {
        // Child Process
        printf("%s:%d Child: Launching Tensor\n", FILENM, __LINE__);
        if (close(pipeFromPy[0]) == -1)
        {
            fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
                "close", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (close(pipeToPy[1]) == -1)
        {
            fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
                "close", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* redirect stdin and stdout */
        if (dup2(pipeFromPy[1], STDOUT_FILENO) == -1)
        {
            fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
                "dup2", strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (dup2(pipeToPy[0], STDIN_FILENO) == -1)
        {
            fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
                "dup2", strerror(errno));
            exit(EXIT_FAILURE);
        }

        /* run Program1 with dup2ed stdout */
        execlp("python Model/trainer.py", "python Model/trainer.py", NULL);

        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "EXECLP", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (close(pipeFromPy[1]) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "close", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (close(pipeToPy[0]) == -1)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "close", strerror(errno));
        exit(EXIT_FAILURE);
    }


    // Send initialization
    printf("%s:%d Initializing Tensor\n", FILENM, __LINE__);
    char buff[8];
    sprintf(buff, "0");
    int bytesWritten = sizeof(char) * strlen(buff);
    if (write(pipeToPy[0], buff, bytesWritten) != bytesWritten)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "write", strerror(errno));
    }

    sprintf(buff, "1");
    bytesWritten = sizeof(char) * strlen(buff);
    if (write(pipeToPy[0], buff, bytesWritten) != bytesWritten)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "write", strerror(errno));
    }

    ctrl = ai;
}

void TensorHandler::SendToPipe(Player ai, Player enemy)
{
    printf("%s:%d Sending Current Data\n", FILENM, __LINE__);
    // Send input
    char buff[256];
    sprintf(buff, "%u %d %f %f %u %d %f %f",
        ai.health, ai.dir, ai.pos_x, ai.pos_y,
        enemy.health, enemy.dir, enemy.pos_x, enemy.pos_y);
    int bytesWritten = sizeof(char) * strlen(buff);
    if (write(pipeToPy[1], buff, bytesWritten) != bytesWritten)
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "write", strerror(errno));

    printf("%s:%d Sent:\n"
        "\t%u %d %f %f %u %d %f %f\n", FILENM, __LINE__,
        ai.health, ai.dir, ai.pos_x, ai.pos_y,
        enemy.health, enemy.dir, enemy.pos_x, enemy.pos_y);
}

std::string TensorHandler::ReadFromPipe()
{
    printf("%s:%d Reading From Pipe\n", FILENM, __LINE__);
    // Get output
    char buff[1024];
    bool hadRead = false;
    unsigned int ret = 0, loop = 0;
    string output;
    do {
        ret = read(pipeFromPy[0], buff, 1023);
        if (ret == -1)
        {
            fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
                "read", strerror(errno));
            return "";
        }
        else
            hadRead = true;
        buff[1023] = '\0'; // Makes sure it's terminated
        printf("%s:%d Read%d %s\n", FILENM, __LINE__, loop++, buff);
        output += buff;
    } while (ret != 0 || !hadRead);

    printf("TH: Tensor output:\n\t%s\n", output.c_str());
    return output;
}

bool TensorHandler::handleController(string tensor)
{
    printf("%s:%d Parsing Tensor Output\n", FILENM, __LINE__);
    float sx, sy;
    int ba, bb, by, bz, bl;
    if (sscanf(tensor.c_str(), "%f %f %d %d %d %d %d",
        &sx, &sy, &ba, &bb, &by, &bz, &bl) < 7)
    {
        fprintf(stderr, "%s:%d: %s\n\t%s\n", FILENM, __LINE__,
            "sscanf failed to parse tensor output", tensor.c_str());
        return false;
    }

    printf("%s:%d Sending Controls to Controller\n", FILENM, __LINE__);
    ctrl->setControls({ sx, sy, ba, bb, by, bz, bl });
    return ctrl->SendState();
}

bool TensorHandler::MakeExchange(MemoryScanner* mem)
{
    printf("%s:%d Making Exchange\n", FILENM, __LINE__);
    SendToPipe(mem->GetPlayer(ctrl->player), mem->GetPlayer(!ctrl->player));

    string ret = ReadFromPipe();
    if (ret.size() == 0)
        return false;

    return handleController(ret);
}

TensorHandler::TensorHandler() :
    pid(-1),
    pipeToPy{ -1,-1 },
    pipeFromPy{ -1,-1 }
{
}

TensorHandler::~TensorHandler()
{
    printf("%s:%d Closing TensorHandler\n", FILENM, __LINE__);
    // Write closing line to Python
    printf("%s:%d Writing Close\n", FILENM, __LINE__);
    char buff[256];
    sprintf(buff, "-1 -1");
    int bytesWritten = sizeof(char) * strlen(buff);
    if (write(pipeToPy[0], buff, bytesWritten) != bytesWritten)
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "write", strerror(errno));

    // read from pipe
    printf("%s:%d Getting CLosing line from Tensor\n", FILENM, __LINE__);
    bool hadRead = false;
    unsigned int ret = 0, loop = 0;
    string output;
    do {
        ret = read(pipeFromPy[0], buff, 255);
        if (ret == -1)
            fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
                "read", strerror(errno));
        else
            hadRead = true;
        buff[255] = '\0'; // Makes sure it's terminated
        printf("%s:%d Read%d %s\n", FILENM, __LINE__, loop++, buff);
        output += buff;
    } while (ret != 0 || !hadRead);

    // Make sure we can find the closing identifier
    if (output.find("-1 -1") == string::npos)
        fprintf(stderr, "%s:%d: %s\n", FILENM, __LINE__,
            "Tensor Did not close properly");

    // close the pipes
    printf("%s:%d Shutting Down Handler\n", FILENM, __LINE__);
    close(pipeFromPy[0]);
    close(pipeToPy[1]);

    // Wait for Tensor to close
    int status;
    int tpid;
    tpid = waitpid(pid, &status, 0);
    printf("%s:%d Tensor:%d closed\n", FILENM, __LINE__, status);
}
