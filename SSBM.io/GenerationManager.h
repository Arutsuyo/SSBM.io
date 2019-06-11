#pragma once
#include "Trainer.h"
#include <string>
#include <mutex>

// Hyper parameters
// TESING, raise to desired amount (200?)
#define SIZE_GEN 100
#define SIZE_CULL_TO 8

// Pathing Info: To be used 
#define DIR_AI_BASE "AI/"
#define DIR_PARENT_BASE "p%d/"
#define DIR_CHILD_BASE "c%d/"


class GenerationManager
{
    // Private constructor disables being able to create an object of this class. 
    // This forces everything it does to be a static Member/Method
    GenerationManager();

    static int childCount;
    static int childDirCount;
    static int parentCount;

    static std::string curParentDir;
    static std::string curChildDir;
    static std::string modelPath;

    static std::mutex mut;
    static bool initialized;
public:
    // TensorHandler Functions
    static std::string GetParentFile();
    static std::string GetChildFile();

    // Trainer Functions
    static bool GenerationReady();
    static bool CullTheWeak();

    // Must be called before everything else
    static bool Initialize(std::string model_path);
};

