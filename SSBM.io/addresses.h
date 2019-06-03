#ifndef _ADDRESSES_H
#define _ADDRESSES_H

namespace Addresses {


    enum PLAYERS {

        PLAYER_ONE = 0x453130,
        PLAYER_TWO = 0x453FC0
    };

    enum MENUS{
        /*offset to get stage */
        MENU_STATE = 0x479d30;

        CHARACTER_SELECT = 33685760,
        STAGE_SELECT = 33685761,
        IN_GAME = 33685762,
        POSTGAME = 33685764   

    };

    enum PLAYER_ATTRIB {

        /*health*/
        P1_HEALTH = 0x4530E0,
        P2_HEALTH = 0x453F70,

        /*stock*/
        P1_STOCK = 0x45310E,
        P2_STOCK = 0x453F9E,

        /*direction*/
        P1_DIR = 0x4530C0,
        P2_DIR = 0x453F50,

        /*X direction*/
        P1_COORD_X = 0x453090,
        P2_COORD_X = 0x453F20,

        /*Y Direction*/
        P1_COORD_Y = 0x453094,
        P2_COORD_Y = 0x453F24

    };

};



#endif