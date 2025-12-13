/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the declaration of the Mushroom and its actions
*/ 

#ifndef MUSHROOM_HPP
#define MUSHROOM_HPP

#include <SFML/Graphics.hpp>
#include "Constants.h"

using namespace sf;

// Purpose: Contain and govern all fields, states, and functions of a single Mushroom in the game
class Mushroom
{

public:
    // Purpose: Constructor
    // @param x -- the x grid coordinate to spawn the Mushroom at
    // @param y -- the y grid coordinate to spawn the Mushroom at
    // @param occupancyGrid -- a pointer to the 2D array detailing what entity is within what grid box
    Mushroom(int x, int y);

    // Purpose: Damage the mushroom
    // @return bool -- True if the mushroom is destroyed, false otherwise
    bool damage();


    //////// Getters

    // Purpose: Getter for the sprite variable
    // @return Sprite * -- pointer to the Mushroom's sprite
    Sprite * getSprite();

    // Purpose: Getter for the x variable
    // @return int -- the x coordinate of the Mushroom
    int getX();

    // Purpose: Getter for the y variable
    // @return int -- the y coordinate of the Mushroom
    int getY();

private:
    // Mushroom texture/sprites
    Texture textureMushroom;
    Sprite spriteMushroom;

    // Grid coordinates for the Mushroom
    int x;
    int y;

    // Mushroom health remaining
    int health;
};

#endif