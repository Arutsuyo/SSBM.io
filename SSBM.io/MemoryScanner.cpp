#include <errno.h>
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
using namespace std;

MemoryScanner::MemoryScanner(string inUserDir)
{

    p1.health = 1000; p2.health = 1000;
    p1.dir = 10; p2.dir = 10;
    p1.pos_y = -1024; p1.pos_x = -1024;
    p2.pos_y = -1024; p2.pos_x = -1024;
    
    userPath = inUserDir;

    init_socket();
}

Player MemoryScanner::GetPlayer(bool pl)
{
    return !pl ? p1 : p2;
}

void MemoryScanner::print()
{
    cout << "MEM:P1: HP:" << p1.health << ", Dir:" << p1.dir << ", X:" << p1.pos_x << ", Y:" << p1.pos_y << endl;
    cout << "MEM:P1: HP:" << p2.health << ", Dir:" << p2.dir << ", X:" << p2.pos_x << ", Y:" << p2.pos_y << endl;
}

/*initializes unix socket in default path....*/
void MemoryScanner::init_socket() {

    string sock_path = userPath + "MemoryWatcher/MemoryWatcher";
    cout << "MEM: Socket Path: " << sock_path << endl;

    /*set up socket*/

    socketfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (socketfd < 0)
        cout << "Socket file descriptor returned with " << socketfd << endl;
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    unlink(sock_path.c_str());

    strncpy(addr.sun_path, sock_path.c_str(), sizeof(addr.sun_path) - 1);

    if (bind(socketfd, (struct sockaddr*) & addr, sizeof(addr)) < 0) {
        cout << "Error binding socket " << strerror(errno) << endl;
        cout << "path is " << sock_path << endl;
        cout << "socketfd is " << socketfd << endl;
    }
}

int MemoryScanner::UpdatedFrame() {

    if (socketfd < 0) {
        cout << "Error socket file descriptor is bad" << endl;
        return -1;
    }

    char buffer[128];
    memset(buffer, '\0', 128);

    uint val_int;

    struct sockaddr recvsock;
    socklen_t sock_len;
    recvfrom(socketfd, buffer, sizeof(buffer), 0, &recvsock, &sock_len);
    //puts(buffer);
    stringstream ss(buffer);
    /*strings to hold addresses*/
    string base, val;

    getline(ss, base, '\n');
    getline(ss, val, '\n');

    /*attempt to find any pointers, should be ' ' but may be a comma
    double check this*/
    size_t pointer_ref = base.find(" ");
    /*check until the end*/
    if (pointer_ref != string::npos) {

        string offset = base.substr(pointer_ref + 1);
        string ptr_base = base.substr(0, pointer_ref);

        /*convert to base16 number*/
        uint ptr = stoul(offset.c_str(), nullptr, 16);
        uint p_base = stoul(ptr_base.c_str(), nullptr, 16);
    }
    else {

        uint player_val = stoul(base.c_str(), nullptr, 16);
        switch (player_val) {

            /*p1 health*/
        case Addresses::PLAYER_ATTRIB::P1_HEALTH: {
            val_int = stoul(val.c_str(), nullptr, 16);
            //cout << "health? " << (val_int / 16) <<endl;
            //val_int = val_int >> 16;
            //cout << "HEALTH " << val_int << endl;
            p1.health = (val_int / 16);
            break;	}
        case Addresses::PLAYER_ATTRIB::P1_COORD_X: {
            val_int = stoul(val.c_str(), nullptr, 16);
            uint* vx = &val_int;
            float x = *((float*)vx);
            p1.pos_x = x;
            //cout << "float x :" << x << endl;
            break; }
        case Addresses::PLAYER_ATTRIB::P1_COORD_Y: {
            val_int = stoul(val.c_str(), nullptr, 16);
            uint* vy = &val_int;
            float y = *((float*)vy);
            p1.pos_y = y;
            //cout << "float y:" << y << endl;
            break; }
        case Addresses::PLAYER_ATTRIB::P1_DIR: {
            val_int = stoul(val.c_str(), nullptr, 16);
            //left 191 right 63
            if (val_int == 191)
                p1.dir = -1;
            if (val_int == 63)
                p1.dir = 1;
            //cout << "facing dir:" << facing << endl;
            break; }
                                               /*P2 */
        case Addresses::PLAYER_ATTRIB::P2_HEALTH:
            val_int = stoul(val.c_str(), nullptr, 16);
            p2.health = (val_int / 16);
            //cout << "HEALTH 2 " << val_int << endl;
            break;
        case Addresses::PLAYER_ATTRIB::P2_COORD_X: {
            val_int = stoul(val.c_str(), nullptr, 16);
            uint* vx = &val_int;
            float x = *((float*)vx);
            p2.pos_x = x;
            //cout << "float 2x: " << x << endl;
            break; }
        case Addresses::PLAYER_ATTRIB::P2_COORD_Y: {
            val_int = stoul(val.c_str(), nullptr, 16);
            uint* vy = &val_int;
            float y = *((float*)vy);
            p2.pos_y = y;
            //cout << "float 2y: " << y << endl;
            break; }
        case Addresses::PLAYER_ATTRIB::P2_DIR: {
            val_int = stoul(val.c_str(), nullptr, 16);
            //left 191 right 63
            if (val_int == 191)
                p2.dir = -1;
            if (val_int == 63)
                p2.dir = 1;
            break; }
        default:
            break;
            /*p2 health*/
        }

        print();
    }

    return 1;
}

