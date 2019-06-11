#include "GenerationManager.h"
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

int GenerationManager::childCount = -1;
int GenerationManager::childDirCount = -1;
int GenerationManager::parentCount = -1;

std::string GenerationManager::curParentDir = "";
std::string GenerationManager::curChildDir = "";


std::mutex Trainer::mut;
bool GenerationManager::initialized = false;

std::string GenerationManager::GetParentFile()
{
    if (!initialized)
    {
        fprintf(stderr, "%s:%d\t--ERROR: GenerationManager MUST BE INITIALIZED\n", FILENM, __LINE__);
        exit(EXIT_FAILURE);
    }
    std::unique_lock<std::mutex> lk(mut);

    // Get a randomized parent file 
    // Or get a parent file based off current child numbers, this way we can 
    // equalize the parent training distribution
    return curParentDir + std::to_string(rand() % SIZE_CULL_TO) + ".h5";
}

std::string GenerationManager::GetChildFile()
{
    if (!initialized)
    {
        fprintf(stderr, "%s:%d\t--ERROR: GenerationManager MUST BE INITIALIZED\n", FILENM, __LINE__);
        exit(EXIT_FAILURE);
    }
    std::unique_lock<std::mutex> lk(mut);

    // Parse the child file based on the model path, dirCount, and ChildCount
    std::string childFile;
    childCount++;
    childFile = curChildDir + std::to_string(childCount);
    childFile += ".h5";

    if (file_exists(childFile.c_str()))
    {
        fprintf(stderr, "%s:%d\t--ERROR: Child file already exists, overwriting: %s\n", FILENM, __LINE__, childFile.c_str());
    }
    return childFile;
}

bool GenerationManager::GenerationReady()
{
    if (!initialized)
    {
        fprintf(stderr, "%s:%d\t--ERROR: GenerationManager MUST BE INITIALIZED\n", FILENM, __LINE__);
        exit(EXIT_FAILURE);
    }
    std::unique_lock<std::mutex> lk(mut);

    // Check if there's enough children to cull
    return childCount >= SIZE_GEN;
}

bool GenerationManager::CullTheWeak()
{
    if (!initialized)
    {
        fprintf(stderr, "%s:%d\t--ERROR: GenerationManager MUST BE INITIALIZED\n", FILENM, __LINE__);
        exit(EXIT_FAILURE);
    }
    std::unique_lock<std::mutex> lk(mut);

    // Warn the user we are going to cull, don't kill the program

    // Initiate culling

    // Make Parent Dir

    // Copy Children into parent

    // Make new child dir

    // Make sure variables are set right

    // Ask the user if they want to start on the next generation
    // This should be a timed prompt, if there's no response, start training
    
}

bool GenerationManager::Initialize(std::string model_path)
{
    std::unique_lock<std::mutex> lk(mut);
    const unsigned int bufflen = 256;
    char buff[bufflen];
    memset(buff, 0, bufflen);
    std::string childFile;
    modelPath = model_path;
    printf("%s:%d\tInitilizing GenerationManager\n",
        FILENM, __LINE__);

    // Getting Child dir Count
    bool fresh = true;
    do
    {
        // Increment Child Dir Count
        childDirCount++;

        // Parse the Next Child Folder
        std::string temp = DIR_AI_BASE;
        temp += modelPath + DIR_CHILD_BASE;
        sprintf(buff, temp.c_str(), childDirCount);
        curChildDir = buff;
        // Test for existence
        if (!dir_exists(curChildDir.c_str()))
        {
            // Set the dir to the previous one
            if (childDirCount != 0)
            {
                // We may not have finished training a generation
                childDirCount--;
                sprintf(buff, temp.c_str(), childDirCount);
                curChildDir = buff;
                break;
            }
            else
            {
                // Fresh Model
                fresh = false;
                break;
            }
        }
    } while (true);

    printf("%s:%d\tReading children in: %s\n",
        FILENM, __LINE__, curChildDir.c_str());

    if (fresh)
    {
        printf("%s:%d\tStarting a fresh AI!\n",
            FILENM, __LINE__);
        // Fresh AI, make the new folder
        sprintf(buff, "mkdir %s", curChildDir.c_str());
        system(buff);
        childCount = 0;
        parentCount = 0;
    }
    else
    {
        // Count the current number of children. This allows us to persist
        do
        {
            childCount++;
            childFile = curChildDir + std::to_string(childCount);
            childFile += ".h5";

        } while (file_exists(childFile.c_str()));

        printf("%s:%d\tCurrent Child Count: %d\n",
            FILENM, __LINE__, childCount);
    }

    // Parse the Parent Folder
    curParentDir = DIR_AI_BASE;
    curParentDir += modelPath + DIR_PARENT_BASE;
    sprintf(buff, curParentDir.c_str(), childDirCount);
    curParentDir = buff;
    if (childCount == SIZE_CULL_TO - 1 && dir_exists(buff))
    {
        // Count the number of parents
        int pCount = -1;
        do
        {
            pCount++;
            childFile = curParentDir + std::to_string(pCount);
            childFile += ".h5";

        } while (file_exists(childFile.c_str()));
        if (pCount != SIZE_CULL_TO - 1)
        {
            fprintf(stderr, "%s:%d\t%s Not enough Parent Files, Copying the correct Children\n", FILENM, __LINE__,
                "--ERROR:GenerationManager", DIR_AI_BASE, modelPath.c_str());
            sprintf(buff, "cp %s* %s*", curChildDir.c_str(), curParentDir.c_str());
            system(buff);
        }
        printf("%s:%d\tFinished Training, ready for next gen!\n",
            FILENM, __LINE__);

        // Increment Child Dir Count
        childDirCount++;

        // Parse the Next Child Folder
        std::string temp = DIR_AI_BASE;
        temp += modelPath + DIR_CHILD_BASE;
        sprintf(buff, temp.c_str(), childDirCount);
        curChildDir = buff;

        // Make the new folder
        sprintf(buff, "mkdir %s", curChildDir.c_str());
        system(buff);
    }
    else if (childCount < SIZE_GEN)
    {
        printf("%s:%d\tDid not finish training, Continuing!\n",
            FILENM, __LINE__);
        // Set the parent dir
        curParentDir = buff;

        // Increment Child Dir Count
        childDirCount++;

        // Parse the Next Child Folder
        std::string temp = DIR_AI_BASE;
        temp += modelPath + DIR_CHILD_BASE;
        sprintf(buff, temp.c_str(), childDirCount);
        curChildDir = buff;
    }
    else
    {
        fprintf(stderr, "%s:%d\t%s Investigate the %s%s folder and fix the code!\n", FILENM, __LINE__,
            "--ERROR:GenerationManager", DIR_AI_BASE, modelPath.c_str());
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    initialized = true;
}
