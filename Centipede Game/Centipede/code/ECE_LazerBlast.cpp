/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the implementation of the Lazer Blast and its actions
*/ 

#include "ECE_LazerBlast.hpp"

// Purpose: Constructor
// @param x -- the x grid coordinate to spawn the Lazer Blast at
// @param y -- the y grid coordinate to spawn the Lazer Blast at
// @param occupancyGrid -- a pointer to the 2D array detailing what entity is within what grid box
ECE_LazerBlast::ECE_LazerBlast(int x, int y, OccupancyGrid * occupancyGrid)
{
    // Save parameters to class variables
    this->occupancyGrid = occupancyGrid;
    this->x = x;
    this->y = y;

    // Setup the lazer blast sprite and texture
    this->textureLazerBlast.loadFromFile("graphics/LazerBlast.png");
    this->spriteLazerBlast.setTexture(textureLazerBlast);
    this->spriteLazerBlast.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y));

    // Place the Lazer Blast on the occupancy grid unless there is something else there (will typically be the player, so do not want to override that)
    if ((*occupancyGrid)[x][y] == NONE)
    {
        (*occupancyGrid)[x][y] = LAZER;
    }

    // Start the movement timeout
    clock.restart();
}

// Purpose: Move the Lazer Blast by 1 grid unit
// @return ObjectType -- the occupancyGrid entry for the space the Lazer Blast is moving into prior to its moving (what, if anything, it is now colliding with)
ObjectType ECE_LazerBlast::move()
{
    // If there has been at least LAZER_MS_PER_MOVE milliseconds since the last time the Lazer Blast moved, move it
    if (clock.getElapsedTime().asMilliseconds() >= LAZER_MS_PER_MOVE)
    {
        // If the lazer blast is present on the occupancyGrid, remove it
        if ((*occupancyGrid)[x][y] == LAZER)
        {
            (*occupancyGrid)[x][y] = NONE;
        }
        
        // Move the lazer blast up 1 grid unit
        --y;

        // If the Lazer Blast oved out of the occupancy grid space, stop movement processing. Return no collisions
        if (y < 0)
        {
            return NONE;
        }

        // Save the type of the object in the space the Lazer Blast has moved into
        ObjectType returnVal = (*occupancyGrid)[x][y];
        
        // Update the Lazer Blast's sprite's location in the game window
        spriteLazerBlast.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y));

        // If there was nothing else in the occuancyGrid cell, put the Laser Blast there
        // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the LAZER typing
        if (returnVal == NONE)
        {
            (*occupancyGrid)[x][y] = LAZER;
        }

        // Reset the Lazer Blast's movement clock
        clock.restart();

        // Return the type of the potential collision
        return returnVal;
    }
    else
    {
        // Smooth motion on screen

        // Update the sprite's position based on prorated movement between the actual backend movement steps between every LAZER_MS_PER_MOVE ms
        int yOffset = (-1 * SPRITE_HEIGHT * clock.getElapsedTime().asMilliseconds()) / LAZER_MS_PER_MOVE;
        spriteLazerBlast.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y) + yOffset);
          
        // Since the Lazer Blast is not actually moving on the grid, no new collisions should be handled. This will only occur on normal movement steps
        return NONE;
    }
}


// Purpose: Getter for the sprite variable
// @return Sprite * -- pointer to the Lazer Blast's sprite
Sprite * ECE_LazerBlast::getSprite()
{
    return &spriteLazerBlast;
}

// Purpose: Getter for the x variable
// @return int -- the x coordinate of the Lazer Blast
int ECE_LazerBlast::getX()
{
    return x;
}

// Purpose: Getter for the y variable
// @return int -- the y coordinate of the Lazer Blast
int ECE_LazerBlast::getY()
{
    return y;
}
