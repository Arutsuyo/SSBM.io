#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <sys/stat.h>
#include <stdint.h>
#include "Trainer.h"
#include "MemoryScanner.h"
#include "addresses.h"

/*
struct player{

	unsigned int health;
	int dir; //left -1 right 1
	float pos x;	
	float pos y;

};
*/


MemoryScanner::MemoryScanner(std::string s){

	p1.health = 1000; p2.health = 1000;
	p1.dir = 10; p2.dir = 10;
	p1.pos_y = -1024; p1.pos_x = -1024;
	p2.pos_y = -1024; p2.pos_x = -1024;
	p =s;


	init_socket();
}

void MemoryScanner::readytoprint(){
	
	/*
	if (this->p1.dir == 10 || this->p2.dir == 10)
		return;
	if (this->p1.pos_x == -1024 || this->p1.pos_y == -1024)
		return;
	if (this->p2.pos_x == -1024 || this->p2.pos_y == -1024)
		return;
	*/
	std::cout << p1.health << "," << p1.dir << "," << p1.pos_x << "," << p1.pos_y << std::endl;
	std::cout << p2.health << "," << p2.dir << "," << p2.pos_x << "," << p2.pos_y << std::endl; 
}

/*initializes unix socket in default path.... later maybe add custom path support/env var*/
void MemoryScanner::init_socket(){

	struct passwd *pw = getpwuid(getuid());

	std::string sock_path =  p + "/MemoryWatcher/MemoryWatcher";
	std::cout << "FJKLFJDKLJFLKDJFKLDJFKLDJ" << sock_path <<std::endl;
	//std::string path = pw->pw_dir;
	//path += "/.dolphin-emu/MemoryWatcher/MemoryWatcher";
	/*set up socket*/

	socketfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if ( socketfd < 0)
		std::cout << "Socket file descriptor returned with " << socketfd << std::endl;
	struct sockaddr_un addr;
	memset(&addr, 0 , sizeof(addr));
	addr.sun_family = AF_UNIX;
	unlink(sock_path.c_str());

	strncpy(addr.sun_path, sock_path.c_str(), sizeof(addr.sun_path) - 1);

		if ( bind(socketfd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
			std::cout << "Error binding socket " << strerror(errno) << std::endl;
			std::cout << "path is " << sock_path << std::endl;
			std::cout << "socketfd is " << socketfd << std::endl;
		} 
}

int MemoryScanner::UpdatedFrame(){

	if ( socketfd < 0){
		std::cout << "Error socket file descriptor is bad" << std::endl;
		return -1;
	}

	char buffer[128];
	memset(buffer, '\0', 128);

	uint val_int;

	struct sockaddr recvsock;
	socklen_t sock_len;
	recvfrom(socketfd , buffer, sizeof(buffer), 0 , &recvsock, &sock_len );
	//puts(buffer);
	std::stringstream ss(buffer);
	/*strings to hold addresses*/
	std::string base, val;

	std::getline( ss , base, '\n');
	std::getline( ss , val, '\n');

	/*attempt to find any pointers, shoud be ' ' but may be a comma
	double check this*/
	std::size_t pointer_ref = base.find(" ");
		/*check until the end*/
	if ( pointer_ref != std::string::npos){

		std::string offset = base.substr(pointer_ref + 1);
		std::string ptr_base = base.substr(0, pointer_ref);
		
					/*convert to base16 number*/
		uint ptr = std::stoul(offset.c_str(), nullptr, 16);
		uint p_base = std::stoul(ptr_base.c_str(), nullptr, 16);
		/*
		switch(ptr_base){

			case(Addresses::PLAYERS::PLAYER_ONE):
				break;

		
		}	
		*/	

	} else {

		uint player_val = std::stoul(base.c_str(), nullptr, 16);
		switch( player_val){

			/*p1 health*/
			case Addresses::PLAYER_ATTRIB::P1_HEALTH :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				//std::cout << "health? " << (val_int / 16) <<std::endl;
				//val_int = val_int >> 16;
				//std::cout << "HEALTH " << val_int << std::endl;
				p1.health = (val_int / 16) ;
				break;	}
			case Addresses::PLAYER_ATTRIB::P1_COORD_X :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				uint* vx = &val_int;
				float x = *((float*)vx);
				p1.pos_x = x;
				//std::cout << "float x :" << x << std::endl;
				break;}
			case Addresses::PLAYER_ATTRIB::P1_COORD_Y :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				uint* vy = &val_int;
				float y = *((float*)vy);
				p1.pos_y = y;
				//std::cout << "float y:" << y << std::endl;
				break;}
			case Addresses::PLAYER_ATTRIB::P1_DIR :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				//left 191 right 63
				if (val_int == 191)
					p1.dir = -1;
				if ( val_int == 63)
					p1.dir = 1;
				//std::cout << "facing dir:" << facing << std::endl;
				break;}
			/*P2 */
			case Addresses::PLAYER_ATTRIB::P2_HEALTH :
				val_int = std::stoul(val.c_str(), nullptr, 16);				
				p2.health = (val_int / 16) ;
				//std::cout << "HEALTH 2 " << val_int << std::endl;
				break;	
			case Addresses::PLAYER_ATTRIB::P2_COORD_X :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				uint* vx = &val_int;
				float x = *((float*)vx);
				p2.pos_x = x;
				//std::cout << "float 2x: " << x << std::endl;
				break;}
			case Addresses::PLAYER_ATTRIB::P2_COORD_Y :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				uint* vy = &val_int;
				float y = *((float*)vy);
				p2.pos_y = y;
				//std::cout << "float 2y: " << y << std::endl;
				break;}
			case Addresses::PLAYER_ATTRIB::P2_DIR :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				//left 191 right 63
				if (val_int == 191)
					p2.dir = -1;
				if ( val_int == 63)
					p2.dir = 1;
				break;}
			default:
				break;
			/*p2 health*/
		}

		readytoprint();		
	}

	return 1;
}


