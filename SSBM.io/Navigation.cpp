#include "Navigation.h"



Navigation::Navigation(Controller& c, MemoryScanner* mem) {
    this->memory = mem;
    this->cont = c;
}
//captain falcon x = 17.4 y =17
//final dest x = -16.4 y =18.87
                        //change later to accept arbitrary values
bool Navigation::FindPos() {

    //might need memory pointer to update the player
    Player p2;
    Player p1;
    bool exit = false;

    while (!exit)
    {
        puts("IN FIRST CHECK");
       p1 = memory->GetPlayer(true);
        p2 = memory->GetPlayer(false);


        /*character select screen*/
        while (p2.current_menu == 3)
        {           
            puts("SELECTING CHARAXTER");
            p1 = memory->GetPlayer(true);
            p2 = memory->GetPlayer(false);
            
            int count = 0;

            if (p1.cursor_x < 16.5)
            {
                puts("LESS");
                count++;
                tilt_right();
            }
            if (p1.cursor_x > 19.4)
            {
                puts("MORE");
                count++;
                tilt_left();
            }
            if (p1.cursor_y > 18)
            {
                count++;
                tilt_up();
            }
            if (p1.cursor_y < 16.5)
            {
                count++;
                tilt_down();
            }

            if (count == 0){
                puts("FOUND");
                return;
            }
            //select_character();
            count = 0;
            memory->UpdatedFrame(false);
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
        memory->UpdatedFrame(false);
        exit = true;
    }
    return true;
}

void Navigation::tilt_right() {
    std::string b = "echo 'SET MAIN .6 .5' >> /home/zach/.dolphin-emu/Pipes/pipe1";
    system(b.c_str());
    //cont.setSticks(.6, .5);
    //(void)cont.SendState();
}

void Navigation::tilt_left() {
    std::string b = "echo 'SET MAIN .4 .5' >> /home/zach/.dolphin-emu/Pipes/pipe1";
    system(b.c_str());
}

void Navigation::tilt_up() {
    std::string b = "echo 'SET MAIN .5 .6' >> /home/zach/.dolphin-emu/Pipes/pipe1";
    system(b.c_str());
}

void Navigation::tilt_down() {
    std::string b = "echo 'SET MAIN .6 .4' >> /home/zach/.dolphin-emu/Pipes/pipe1";
    system(b.c_str());
}