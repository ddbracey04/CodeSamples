/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24
Description: 
This file contains all common constants/types/helper functions that are used by other files
*/ 

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Dimensions of regions in play window (in grid units)
#define MAIN_WINDOW_WIDTH 29
#define GAME_AREA_HEIGHT 19
#define INFO_AREA_HEIGHT 2
#define BOTTOM_AREA_HEIGHT 2
#define MAIN_WINDOW_HEIGHT (GAME_AREA_HEIGHT + INFO_AREA_HEIGHT + BOTTOM_AREA_HEIGHT)


// Scores for each hit
#define SCORE_MUSHROOM 4
#define SCORE_CENTIPEDE_HEAD 100
#define SCORE_CENTIPEDE_BODY 10
#define SCORE_SPIDER 300

// Padding pixels on Top and Left of screen
#define X_WINDOW_PADDING 5
#define Y_WINDOW_PADDING 5

// Grid unit/Sprite size
#define SPRITE_WIDTH 27
#define SPRITE_HEIGHT 26

// Mushroom constants
#define NUM_MUSHROOMS 30
#define MAX_MUSHROOM_HEALTH 2

// Player constants
#define MAX_PLAYER_HEALTH 3
#define PLAYER_MOVEMENT_AREA_HEIGHT 8

// Movement speeds (ms per grid unit)
#define LAZER_MS_PER_MOVE 20
#define CENTIPEDE_MS_PER_MOVE 100
#define SPIDER_MS_PER_MOVE 150
#define LAZER_FIRE_TIMEOUT_MS 100

// Centipede constants
#define CENTIPEDE_MAX_LENGTH 11

// Spider constants
#define SPIDER_SPAWN_CHANCE 1
#define SPIDER_RESPAWN_TIMEOUT_MS 3000
#define SPIDER_HORIZONTAL_MOVE_CHANCE 20


// Miscellaneous constants
#define BACKGROUND_COLOR 0x0c171dff


// Types used for determining what entity occupies each grid box
typedef enum
{
    NONE = 0,
    LAZER = 1,
    MUSHROOM = 2,
    PLAYER = 3,
    CENTIPEDE = 4,
    SPIDER = 5
} ObjectType;

typedef ObjectType OccupancyGrid[MAIN_WINDOW_WIDTH][INFO_AREA_HEIGHT + GAME_AREA_HEIGHT + BOTTOM_AREA_HEIGHT];


// Helper functions to calculate screen coordinates (in pixels) from grid coordinates

// Purpose: Calculate the x pixel coordinate on the screen of the top left of a given grid block based on its x grid coordinate
// @param x -- x grid coordinate
// @return float -- the translated x coordinate in pixel space/coordinates
inline float getScreenXFromGrid(int x)
{
    return (float) (x * SPRITE_WIDTH) + X_WINDOW_PADDING;
}

// Purpose: Calculate the y pixel coordinate on the screen of the top left of a given grid block based on its y grid coordinate
// @param y -- y grid coordinate
// @return float -- the translated y coordinate in pixel space/coordinates
inline float getScreenYFromGrid(int y)
{
    // NOTE: y=0 is first line in main game area, not the top of the screen, so there is an INFO_AREA_HEIGHT offset
    return (float) (y * SPRITE_HEIGHT) + Y_WINDOW_PADDING;
}

#endif