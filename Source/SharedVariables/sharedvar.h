
#ifndef __SHAREDVAR_
#define __SHAREDVAR_

#include "LPC17xx.h"
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h> 
#include <time.h>   

#endif


/*********COSTANTS*********/
#define PACMAN_DIM 20
#define ROWS 48
#define COLS 64
#define CELL_DIM 5
#define PILL_INC 10
#define SUPER_PILL_INC 50
#define SUPER_PILL_NUMBER 6
#define LIFE_DIM 15
#define LIFE_GAIN_POINTS 1000
#define PAUSE_X_POS ((ROWS/2)-4)
#define PAUSE_Y_POS ((COLS/2)-2)
#define GAMEOVER_X_POS ((ROWS/2)-8) //OR VICTORY
#define GAMEOVER_Y_POS ((COLS/2)-2)	//OR VICTORY
#define START_PAUSE_X_POS ((ROWS/2)-15)
#define START_PAUSE_Y_POS ((COLS/2)-2)
#define GAMEOVER_Y_POS ((COLS/2)-2)	//OR VICTORY
#define INIT_TIME 60
#define INIT_LIFES 1
/*********VARIABLES*********/
extern uint8_t counterPointer;
extern int counterScore;
extern uint8_t counterTime;
extern uint8_t counterLifes;
extern bool pause; 
extern bool start; 
extern int seed;
extern int life_position[2];
extern uint8_t life_matrix [LIFE_DIM][LIFE_DIM];
extern enum CardinalPoint FACING;
extern uint8_t matrix_pacman[PACMAN_DIM][PACMAN_DIM];
extern int8_t matrix_map[ROWS][COLS];
extern int pacman_position_x[2];
extern int pacman_position_y[2];
extern uint8_t wallFull[CELL_DIM][CELL_DIM];
extern uint8_t wallLeftTopCorner[CELL_DIM][CELL_DIM];
extern uint8_t wallRightTopCorner[CELL_DIM][CELL_DIM];
extern uint8_t wallRightDownCorner[CELL_DIM][CELL_DIM];
extern uint8_t wallLeftDownCorner[CELL_DIM][CELL_DIM];

extern uint8_t pointLeftTopCorner[CELL_DIM][CELL_DIM];
extern uint8_t pointRightTopCorner[CELL_DIM][CELL_DIM];
extern uint8_t pointRightDownCorner[CELL_DIM][CELL_DIM];
extern uint8_t pointLeftDownCorner[CELL_DIM][CELL_DIM];
extern uint16_t superPillsCoordinates[SUPER_PILL_NUMBER][2];
extern uint8_t superPointLeftTopCorner[CELL_DIM][CELL_DIM];
extern uint8_t superPointRightTopCorner[CELL_DIM][CELL_DIM];
extern uint8_t superPointRightDownCorner[CELL_DIM][CELL_DIM];
extern uint8_t superPointLeftDownCorner[CELL_DIM][CELL_DIM];
extern uint16_t timesVector[SUPER_PILL_NUMBER];
extern uint16_t superPillsToGenerate; 
extern bool empty; 
extern unsigned short AD_current; 
extern bool mouthOpen; 
/*********VAR TYPES*********/
enum CardinalPoint {
    SOUTH,  
    EST,    
    NORTH,  
    OVEST   
};
