/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the implementation of the Mushroom and its actions
*/ 

#include "Mushroom.hpp"

// Purpose: Constructor
// @param x -- the x grid coordinate to spawn the Mushroom at
// @param y -- the y grid coordinate to spawn the Mushroom at
// @param occupancyGrid -- a pointer to the 2D array detailing what entity is within what grid box
Mushroom::Mushroom(int x, int y)
{
    // Save parameters to class variables
    this->x = x;
    this->y = y;
    this->health = MAX_MUSHROOM_HEALTH;

    // Setup the mushroom sprite and texture
    this->textureMushroom.loadFromFile("graphics/Mushroom0.png");
    this->spriteMushroom.setTexture(textureMushroom);
    this->spriteMushroom.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y));
}

// Purpose: Damage the mushroom
// @return bool -- True if the mushroom is destroyed, false otherwise
bool Mushroom::damage()
{
    // Decrement the health
    --health;

    // If the health is 0 or less, mark the mushroom for destruction
    if (health < 1)
    {
        return true;
    }

    // If mushroom is surviving, set the texture to the damaged mushroom
    textureMushroom.loadFromFile("graphics/Mushroom1.png");
    spriteMushroom.setTexture(textureMushroom); 
    return false;
}


// Purpose: Getter for the sprite variable
// @return Sprite * -- pointer to the Mushroom's sprite
Sprite * Mushroom::getSprite()
{
    return &this->spriteMushroom;
}

// Purpose: Getter for the x variable
// @return int -- the x coordinate of the Mushroom
int Mushroom::getX()
{
    return x;
}

// Purpose: Getter for the y variable
// @return int -- the y coordinate of the Mushroom
int Mushroom::getY()
{
    return y;
}