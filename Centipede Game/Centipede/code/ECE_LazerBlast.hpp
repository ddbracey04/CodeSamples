/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the declaration of the Lazer Blast and its actions
*/ 

#ifndef ECE_LAZERBLAST_HPP
#define ECE_LAZERBLAST_HPP

#include <SFML/Graphics.hpp>
#include "Constants.h"

using namespace sf;

// Purpose: Contain and govern all fields, states, and functions of a single Lazer Blast in the game
class ECE_LazerBlast
{

public:
    // Purpose: Constructor
    // @param x -- the x grid coordinate to spawn the Lazer Blast at
    // @param y -- the y grid coordinate to spawn the Lazer Blast at
    // @param occupancyGrid -- a pointer to the 2D array detailing what entity is within what grid box
    ECE_LazerBlast(int x, int y, OccupancyGrid * occupancyGrid);

    // Purpose: Move the Lazer Blast by 1 grid unit
    // @return ObjectType -- the occupancyGrid entry for the space the Lazer Blast is moving into prior to its moving (what, if anything, it is now colliding with)
    ObjectType move();


    //////// Getters

    // Purpose: Getter for the sprite variable
    // @return Sprite * -- pointer to the Lazer Blast's sprite
    Sprite * getSprite();

    // Purpose: Getter for the x variable
    // @return int -- the x coordinate of the Lazer Blast
    int getX();

    // Purpose: Getter for the y variable
    // @return int -- the y coordinate of the Lazer Blast
    int getY();

private:

    // Pointer to the occupancyGrid for the game to reference location and determine collisions
    OccupancyGrid * occupancyGrid;

    // Movement clock to space out movement actions for the Lazer Blast
    Clock clock;

    // Lazer Blast texture/sprites
    Texture textureLazerBlast;
    Sprite spriteLazerBlast;

    // Grid coordinates for the Lazer Blast
    int x;
    int y;
};

#endif