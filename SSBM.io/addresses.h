#ifndef _ADDRESSES_H
#define _ADDRESSES_H

namespace Addresses {


    enum PLAYERS {

        PLAYER_ONE = 0x453130,
        PLAYER_TWO = 0x453FC0
    };

    enum MENUS{
        /*offset to get stage */
        MENU_STATE = 0x479d30,

        CHARACTER_SELECT = 3328,
        STAGE_SELECT = 3329,
        IN_GAME = 3330,
        POSTGAME = 3332   

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
        P2_COORD_Y = 0x453F24,

        /*Cursor */
        P1_CURSOR_X = 0x01118DEC,
        P1_CURSOR_Y = 0x01118DF0,
        P2_CURSOR_X = 0x0111826C,
        P2_CURSOR_Y = 0x01118270,

        /*stage selection cursors */
        STAGE_SELECT_X1 = 0x0c8ee38,
        STAGE_SELECT_X2 = 0x0c8ee50,
        STAGE_SELECT_Y1 = 0x0C8EE3C,
        STAGE_SELECT_Y2 = 0x0C8EE60

    };

    enum CHARACTERS {
        DR_MARIO = 0,
        MARIO = 1,
        LUIGI = 2,
        BOWSER = 3,
        PEACH = 4,
        YOSHI = 5,
        DK = 6,
        C_FALCON = 7,
        GANONDORF = 8,
        FALCO = 9,
        FOX = 10,
        NESS = 11,
        ICE_CLIMBERS = 12,
        KIRBY = 13,
        SAMUS = 14,
        ZELDA = 15,
        LINK = 16,
        YOUNG_LINK = 17,
        PICHU = 18,
        PIKACHU = 19,
        JIGGLYPUFF = 20,
        MEWTWO = 21,
        MR_GAME_AND_WATCH = 22,
        MARTH = 23,
        ROY = 24
    };

};



#endif
