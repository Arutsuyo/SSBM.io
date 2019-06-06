#include <errno.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <sys/stat.h>
#include <stdint.h>
#include "MemoryScanner.h"
#include "addresses.h"
#include <algorithm>
#define FILENM "MS"

MemoryScanner::MemoryScanner(std::string inUserDir)
{
    /*initialize structs to default/trash values*/
    p1.health = 1000; p2.health = 1000;
    p1.dir = 10; p2.dir = 10;
    p1.pos_y = -1024; p1.pos_x = -1024;
    p2.pos_y = -1024; p2.pos_x = -1024;

    userPath = inUserDir;
    success = init_socket();

    printf("%s:%d\tMemory Scanner Created\n", FILENM, __LINE__);
}

MemoryScanner::~MemoryScanner() {

    printf("%s:%d\tDestroying MemoryScanner\n", FILENM, __LINE__);
    /*shutdown the socket*/
    if (shutdown(this->socketfd, 2) < 0)
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "--ERROR:shutdown", strerror(errno));
    printf("%s:%d\tSocket Closed\n", FILENM, __LINE__);
}

Player MemoryScanner::GetPlayer(bool pl)
{
    return !pl ? p1 : p2;
}

bool MemoryScanner::print()
{

    /*quick check for all values to be updated before being sent to model*/
    if (p1.dir == 10 || p2.dir == 10)
    {
        printf("%s:%d\t--Invalid Data: %d:%d\n", FILENM, __LINE__,
            p1.dir, p2.dir);
        return false;
    }
    if (p1.pos_x == -1024 || p1.pos_y == -1024)
    {
        printf("%s:%d\t--Invalid Data: %f:%f\n", FILENM, __LINE__,
            p1.pos_x, p1.pos_y);
        return false;
    }
    if (p2.pos_x == -1024 || p2.pos_y == -1024)
    {
        printf("%s:%d\t--Invalid Data: %f:%f\n", FILENM, __LINE__,
            p2.pos_x, p2.pos_y);
        return false;
    }

    printf("%s:%d\tMemory Scan\n"
        "\tP1:%u P1:%d P1:%f P1:%f\n", FILENM, __LINE__,
        p1.health, p1.dir, p1.pos_x, p1.pos_y);
    printf("\tP2:%u P2:%d P2:%f P2:%f\n",
        p2.health, p2.dir, p2.pos_x, p2.pos_y);
    return true;
}

/*initializes unix socket in default path.... later maybe add custom path support/env var*/
bool MemoryScanner::init_socket() {

    printf("%s:%d\tCreating Socket\n", FILENM, __LINE__);
    std::string sock_path = userPath + "MemoryWatcher/MemoryWatcher";
    printf("%s:%d\tSocket Path: %s\n", FILENM, __LINE__, sock_path.c_str());


    /*set up socket*/
    if ((socketfd = socket(AF_UNIX, SOCK_DGRAM | SOCK_NONBLOCK, 0)) == -1)
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "--ERROR:socket", strerror(errno));

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    unlink(sock_path.c_str());

    strncpy(addr.sun_path, sock_path.c_str(), sizeof(addr.sun_path) - 1);

    if (bind(socketfd, (struct sockaddr*) & addr, sizeof(addr)) < 0)
    {
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "--ERROR:bind", strerror(errno));
        return false;
    }
    return true;
}

bool MemoryScanner::UpdatedFrame(bool prin) {
    /*if (prin)
        printf("%s:%d\tUpdating Memory\n", FILENM, __LINE__);*/

    if (socketfd < 0) {
        std::cout << "Error socket file descriptor is bad" << std::endl;
        return false;
    }

    int ret = -1;
    char buffer[128];
    memset(buffer, '\0', 128);

    unsigned int val_int;

    /*if (prin)
        printf("%s:%d\tReading Socket\n", FILENM, __LINE__);*/
    struct sockaddr recvsock;
    socklen_t sock_len;
    if ((ret = recvfrom(socketfd, 
        buffer, sizeof(buffer), 0, &recvsock, &sock_len)) == -1)
    {
        // Check if the socket is just empty
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            // Nothing there
            if (prin)
            {
                /*printf("%s:%d\tNothing Read: ret%d errno:%d(%d:%d)\n", FILENM, __LINE__, ret, errno, EAGAIN, EWOULDBLOCK);*/

            }
            return true;
        }

        // Nope, we error'd
        fprintf(stderr, "%s:%d: %s: %s\n", FILENM, __LINE__,
            "--ERROR:recvfrom", strerror(errno));
        return false;
    }

    if (prin)
    {
        printf("%s:%d\tParsing Buffer: %s\n", FILENM, __LINE__, buffer);
        
    }

    //puts(buffer);
    std::stringstream ss(buffer);
    /*strings to hold addresses*/
    std::string base, val;


    getline(ss, base, '\n');
    getline(ss, val, '\n');



    /* to fix any issues with cross platform memory reading, remove commas*/
    val.erase(std::remove(val.begin(), val.end(), ','), val.end());

    /*attempt to find any pointers, should be ' ' but may be a comma
    double check this*/
    size_t pointer_ref = base.find(" ");
    /*check until the end*/
    if (pointer_ref != std::string::npos) {
        std::string offset = base.substr(pointer_ref + 1);
        std::string ptr_base = base.substr(0, pointer_ref);

        /*convert to base16 number*/
        unsigned int ptr = std::stoul(offset.c_str(), nullptr, 16);
        unsigned int p_base = std::stoul(ptr_base.c_str(), nullptr, 16);
        /*

        this would check player stances etc, right now not needed,
        but keeping it in for later

        switch(ptr_base){
        */
    }
    else {

        unsigned int player_val = std::stoul(base.c_str(), nullptr, 16);
        switch (player_val) {
            /*p1 health*/
        case Addresses::PLAYER_ATTRIB::P1_HEALTH: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            p1.health = val_int >> 16;
            break;  }
        case Addresses::PLAYER_ATTRIB::P1_COORD_X: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            unsigned int* vx = &val_int;
            float x = *((float*)vx);
            p1.pos_x = x;
            break; }
        case Addresses::PLAYER_ATTRIB::P1_COORD_Y: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            unsigned int* vy = &val_int;
            float y = *((float*)vy);
            p1.pos_y = y;
            break; }
        case Addresses::PLAYER_ATTRIB::P1_DIR: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            
            unsigned int* d1 = &val_int;
            float dir1 = *((float*)d1);
            p1.dir = dir1;

            break; }
                                               /*P2 */
        case Addresses::PLAYER_ATTRIB::P2_HEALTH:
            val_int = std::stoul(val.c_str(), nullptr, 16);
            p2.health = (val_int >> 16);
            break;
        case Addresses::PLAYER_ATTRIB::P2_COORD_X: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            unsigned int* vx = &val_int;
            float x = *((float*)vx);
            p2.pos_x = x;
            break; }
        case Addresses::PLAYER_ATTRIB::P2_COORD_Y: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            unsigned int* vy = &val_int;
            float y = *((float*)vy);
            p2.pos_y = y;

            break; }
        case Addresses::PLAYER_ATTRIB::P2_DIR: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            unsigned int* d2 = &val_int;
            float dir2 = *((float*)d2);
            p2.dir = dir2;
            break; }

        case Addresses::MENUS::MENU_STATE:
            unsigned int z; //z holds value we need

            sscanf(val.substr(val.size()-3).c_str(), "%x", &z);

            switch (z) {
            case Addresses::MENUS::IN_GAME:
                if (prin)
                    printf("%s:%d\tState: In Game\n", FILENM, __LINE__);
                this->current_stage = 1;
                break;
            case Addresses::MENUS::POSTGAME:
                if (prin)
                    printf("%s:%d\tState: Post-game menu\n", FILENM, __LINE__);
                this->current_stage = 2;
                break;
            case Addresses::MENUS::CHARACTER_SELECT:
                if (prin)
                    printf("%s:%d\tState: Character Select\n", FILENM, __LINE__);
                this->current_stage = 3;
                break;
            case Addresses::MENUS::STAGE_SELECT:
                if (prin)
                    printf("%s:%d\tState: Stage Select\n", FILENM, __LINE__);
                this->current_stage = 4;
                break;
            default:
                fprintf(stderr, "%s:%d\t%s\n", FILENM, __LINE__,
                    "--ERROR:Menu offset read, but returned unknown value");
                break;
            }
            break;

        case Addresses::PLAYER_ATTRIB::P1_CURSOR_X: {
            val_int = std::stoul(val.c_str(), nullptr, 16);

            unsigned int* cx = &val_int;
            float cursx = *((float*)cx);
            p1.cursor_x = cursx;
            break; }

        case Addresses::PLAYER_ATTRIB::P1_CURSOR_Y: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            unsigned int* cy = &val_int;
            float cursy = *((float*)cy);
            p1.cursor_y = cursy;
            break; }

        case Addresses::PLAYER_ATTRIB::P2_CURSOR_X: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            printf("P1 CURSOR\n");
            unsigned int* cx = &val_int;
            float cursx = *((float*)cx);
            p2.cursor_x = cursx;
            break; }

        case Addresses::PLAYER_ATTRIB::P2_CURSOR_Y: {
            val_int = std::stoul(val.c_str(), nullptr, 16);
            unsigned int* cy = &val_int;
            float cursy = *((float*)cy);
            p2.cursor_y = cursy;
            break; }

        default:
            break;
        }


        /*only print information if we are in game*/
        if (this->in_game)
            print();
    }


    return true;
}

