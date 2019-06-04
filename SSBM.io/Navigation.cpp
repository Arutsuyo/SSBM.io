#include "Navigation.h"



Navigation::Navigation(Controller &c, MemoryScanner* mem){
	this->memory = mem;
	this->cont = c;
}
//captain falcon x = 17.4 y =17
//final dest x = -16.4 y =18.87
						//change later to accept arbitrary values
bool Navigation::FindPos(){

	//might need memory pointer to update the player

	bool exit = false;

	while( !exit )
	{

		this->p1 = p1;
		this->p2 = p2;
		puts("IN NAVIGATION");
		printf("%.2f is X POS\n",p2->cursor_x );
		printf("Menu is %d\n",p2->current_menu);
				/*character select screen*/
		while( p2->current_menu == 3 )
		{	
			int count = 0;

			if ( p2->cursor_x < 16.5 )
			{	
				puts("LESS");
				count++;
				tilt_right();
			}
			if ( p2->cursor_x > 19.4)
			{
				puts("MORE");
				count++;
				tilt_left();
			}
			if (p2->cursor_y > 18)
			{
				count++;
				tilt_up();
			}
			if (p2->cursor_y < 16.5)
			{
				count++;
				tilt_down();
			}

			if( count == 0)
				puts("FOUND");
				//select_character();
			count = 0;
		}
		/*
		while( p1->current_menu == 4)
		{
			if (p2->cursor_x < -16.4)
				//move right
			if (p2->cursor_x > 16.4)
				//move left
			if (p2->cursor_y > 18.87)
				//move down
			if (p2->cursor_y < 18.87)
				//move up
		}*/
		//clear input
		exit = true;
	}
	return true;
}

void Navigation::tilt_right(){
	cont.setSticks(.6, .5);
	(void)cont.SendState();
}

void Navigation::tilt_left(){
	cont.setSticks(.4, .5);
	(void)cont.SendState();
}

void Navigation::tilt_up(){
	cont.setSticks(.5, .6);
	(void)cont.SendState();
}

void Navigation::tilt_down(){
	cont.setSticks(.5, .4);
	(void)cont.SendState();
}