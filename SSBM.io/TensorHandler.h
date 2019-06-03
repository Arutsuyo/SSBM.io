#pragma once
#include "MemoryScanner.h"
#include "Player.h"

class TensorHandler
{
    int pid;

    // Pipes: 0 is read, 1 is write
    int pipeToPy[2];
    int pipeFromPy[2];

    // Pipe interactions
    void SendToPipe(Player& ai, Player& enemy);
    std::string ReadFromPipe();

public:
    // Must be called before exchanges can be made
    bool CreatePipes();

    // Returns the output from the model in the following format: 
    // 
    // If there's an error, returns ""
    std::string MakeExchange(Player ai, Player enemy);

    TensorHandler();
    ~TensorHandler();
};

