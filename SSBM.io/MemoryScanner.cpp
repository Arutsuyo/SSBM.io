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


MemoryScanner::MemoryScanner(std::string s){

	/*initialize structs to default/trash values*/
	p1.health = 1000; p2.health = 1000;
	p1.dir = 10; p2.dir = 10;
	p1.pos_y = -1024; p1.pos_x = -1024;
	p2.pos_y = -1024; p2.pos_x = -1024;
	p =s;

	if ( init_socket() == true )
		this->success = true;
}

void MemoryScanner::readytoprint(){
	
	/*quick check for all values to be updated before being sent to model*/

	if (this->p1.dir == 10 || this->p2.dir == 10)
		return;
	if (this->p1.pos_x == -1024 || this->p1.pos_y == -1024)
		return;
	if (this->p2.pos_x == -1024 || this->p2.pos_y == -1024)
		return;
	
	std::cout << p1.health << "," << p1.dir << "," << p1.pos_x << "," << p1.pos_y << std::endl;
	std::cout << p2.health << "," << p2.dir << "," << p2.pos_x << "," << p2.pos_y << std::endl; 
}


MemoryScanner::~MemoryScanner(){

	/*shutdown the socket*/
	if (shutdown(this->socketfd , 2) < 0)
		std::cout << strerror(errno) << std::endl;

}

/*initializes unix socket in default path.... later maybe add custom path support/env var*/
bool MemoryScanner::init_socket(){

	struct passwd *pw = getpwuid(getuid());

	std::string sock_path =  p + "MemoryWatcher/MemoryWatcher";

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
			return false;
		} 
	return true;
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

		this would check player stances etc, right now not needed, but keepingit in 
		for later

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
				p1.health = val_int >> 4 ;
				break;	}
			case Addresses::PLAYER_ATTRIB::P1_COORD_X :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				uint* vx = &val_int;
				float x = *((float*)vx);
				p1.pos_x = x;
				break;}
			case Addresses::PLAYER_ATTRIB::P1_COORD_Y :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				uint* vy = &val_int;
				float y = *((float*)vy);
				p1.pos_y = y;
				break;}
			case Addresses::PLAYER_ATTRIB::P1_DIR :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				//left 191 right 63
				if (val_int == 191)
					p1.dir = -1;
				if ( val_int == 63)
					p1.dir = 1;
				break;}
			/*P2 */
			case Addresses::PLAYER_ATTRIB::P2_HEALTH :
				val_int = std::stoul(val.c_str(), nullptr, 16);				
				p2.health = (val_int >> 4) ;
				break;	
			case Addresses::PLAYER_ATTRIB::P2_COORD_X :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				uint* vx = &val_int;
				float x = *((float*)vx);
				p2.pos_x = x;
				break;}
			case Addresses::PLAYER_ATTRIB::P2_COORD_Y :{
				val_int = std::stoul(val.c_str(), nullptr, 16);
				uint* vy = &val_int;
				float y = *((float*)vy);
				p2.pos_y = y;
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


