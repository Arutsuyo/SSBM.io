#pragma once
#include "MemoryScanner.h"
#include "Controller.h"

class TensorHandler
{
    int pid;

    // Pipes: 0 is read, 1 is write
    int pipeToPy[2];
    int pipeFromPy[2];

    Controller* ctrl;

    // Pipe interactions
    void SendToPipe(Player ai, Player enemy);
    std::string ReadFromPipe();
    bool handleController(std::string tensor);

public:
    // Must be called before exchanges can be made
    bool CreatePipes(Controller* ai);

    // Returns the output from the model in the following format: 
    // 
    // If there's an error, returns ""
    bool MakeExchange(MemoryScanner *mem);

    TensorHandler();
    ~TensorHandler();
};

