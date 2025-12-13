/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the implementation of the Spider and its actions
*/ 

#include "Spider.hpp"
#include <random>

// Purpose: Constructor
// @param occupancyGrid -- a pointer to the 2D array detailing what entity is within what grid box
Spider::Spider(OccupancyGrid * occupancyGrid)
{
    // Save pointer to occupancy grid to class variable
    this->occupancyGrid = occupancyGrid;

    // Reset the player to set up default values
    reset();
}

// Purpose: Reset the spider object to its default state with a given health
void Spider::reset()
{
    // Establish a uniform_int_distribution random number generator with range 0-3
    std::uniform_int_distribution<int> distribution(0, 3);
    int rngGen = distribution(generator);

    // Spawn the spider at a random corner
    // For each case, set initial position and initial movement directions
    switch(rngGen)
    {
        case 0: // Bottom left corner
            this->x1 = 0;
            this->x2 = 1;
            this->y = MAIN_WINDOW_HEIGHT - 1;
            this->movementDeltaX = 1;
            this->movementDeltaY = -1;
            break;
        
        case 1: // Top left corner
            this->x1 = 0;
            this->x2 = 1;
            this->y = MAIN_WINDOW_HEIGHT - PLAYER_MOVEMENT_AREA_HEIGHT - 1;
            this->movementDeltaX = 1;
            this->movementDeltaY = 1;
            break;
        
        case 2: // Bottom right corner
            this->x1 = MAIN_WINDOW_WIDTH - 2;
            this->x2 = MAIN_WINDOW_WIDTH - 1;
            this->y = MAIN_WINDOW_HEIGHT - 1;
            this->movementDeltaX = -1;
            this->movementDeltaY = -1;
            break;
        
        case 3: // Top right corner
            this->x1 = MAIN_WINDOW_WIDTH - 2;
            this->x2 = MAIN_WINDOW_WIDTH - 1;
            this->y = MAIN_WINDOW_HEIGHT - PLAYER_MOVEMENT_AREA_HEIGHT - 1;
            this->movementDeltaX = -1;
            this->movementDeltaY = 1;
            break;

    }

    // Set the spider's initial next horizontal motion
    std::uniform_int_distribution<int> percentileDistribution(1, 100);
    rngGen = percentileDistribution(generator);
    if (rngGen <= SPIDER_HORIZONTAL_MOVE_CHANCE)
    {
        nextMovementX = movementDeltaX;
    }

    // Start the spider as inactive
    this->isActive = false;

    // Setup the spider sprite and texture
    this->textureSpider.loadFromFile("graphics/spider.png");
    spriteSpider.setTexture(textureSpider);
    spriteSpider.setPosition(getScreenXFromGrid(x1), getScreenYFromGrid(y));
    
    // Reset the movement and respawn clocks
    clock.restart();
    respawnClock.restart();
}

// Purpose: Damage the spider and remove it from the occupancy grid
void Spider::damage()
{
    // Remove the left side of the spider from the occupancy grid if it is there
    if ((*occupancyGrid)[x1][y] == SPIDER)
    {
        (*occupancyGrid)[x1][y] = NONE;
    }

    // Remove the right side of the spider from the occupancy grid if it is there
    if ((*occupancyGrid)[x2][y] == SPIDER)
    {
        (*occupancyGrid)[x2][y] = NONE;
    }

    // Set the spider to inactive
    isActive = false;

}

// Purpose: Move the spider vertically or diagonally by 1 grid unit
// @return ObjectType * -- A 2-entry array with the collision for both cells of the spider
ObjectType * Spider::move()
{
    // Set the return array to the default values
    collisionType[0] = NONE;
    collisionType[1] = NONE;

    // If there has been at least SPIDER_MS_PER_MOVE milliseconds since the last time the Spider moved, move it
    if (clock.getElapsedTime().asMilliseconds() >= SPIDER_MS_PER_MOVE)
    {
        // Establish a new distribution (1-100) to use to evaluate percentage probabilities against
        std::uniform_int_distribution<int> distribution(1, 100);
        
        // If the spider is inactive, potentially respawn it
        if (isActive == false)
        {
            // If the spider has been inactive for at least SPIDER_RESPAWN_TIMEOUT_MS ms, there is a SPIDER_SPAWN_CHANCE% chance to spawn it
            if (respawnClock.getElapsedTime().asMilliseconds() >= SPIDER_RESPAWN_TIMEOUT_MS && distribution(generator) <= SPIDER_SPAWN_CHANCE)
            {
                // Reset the spider, set it active, and restart the respawn clock
                reset();
                isActive = true;
                respawnClock.restart();
            }

            // Return the default collision ([NONE, NONE])
            return collisionType;
        }


        // If the left side of the spider is present on the occupancyGrid, remove it
        if ((*occupancyGrid)[x1][y] == SPIDER)
        {
            (*occupancyGrid)[x1][y] = NONE;
        }

        // If the right side of the spider is present on the occupancyGrid, remove it
        if ((*occupancyGrid)[x2][y] == SPIDER)
        {
            (*occupancyGrid)[x2][y] = NONE;
        }


        // If the left side is on the left boundary or the right side is on the right boundary of the game area, reverse horizontal movement direction
        if (x1 == 0)
        {
            movementDeltaX = 1;
        } 
        else if (x2 == MAIN_WINDOW_WIDTH - 1)
        {
            movementDeltaX = -1;
        }

        // If the spider is at the top or bottom of the player movement area, reverse vertical movement direction
        if (y == MAIN_WINDOW_HEIGHT - PLAYER_MOVEMENT_AREA_HEIGHT)
        {
            movementDeltaY = 1;
        }
        else if (y == MAIN_WINDOW_HEIGHT - 1)
        {
            movementDeltaY = -1;
        }

        // Apply the previously determined x movement (0 or 1 space in current direction)
        x1 += nextMovementX;
        x2 += nextMovementX;

        // Generate next x movement (applies to next movement command so that movement can be smoothed between movement actions)
        // Move the spider horizontally SPIDER_HORIZONTAL_MOVE_CHANCE% of the time 
        int rngGen = distribution(generator);
        if (rngGen <= SPIDER_HORIZONTAL_MOVE_CHANCE)
        {
            nextMovementX = movementDeltaX;
        }
        else
        {
            nextMovementX = 0;
        }

        // Move the spider vertically
        y += movementDeltaY;
        
        // NOTE: The sprite position will be updated in the next loop cycle after the backend movement has been completed, so no need to update here

        // Save the types of the objects in the space the Spider has moved into
        collisionType[0] = (*occupancyGrid)[x1][y];
        collisionType[1] = (*occupancyGrid)[x2][y];


        // If there was nothing else besides a Mushroom in the occuancyGrid cell, put the left side of the Spider there
        // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the SPIDER typing
        if ((*occupancyGrid)[x1][y] == NONE || (*occupancyGrid)[x1][y] == MUSHROOM)
        {
            (*occupancyGrid)[x1][y] = SPIDER;
        }

        // If there was nothing else besides a Mushroom in the occuancyGrid cell, put the left side of the Spider there
        // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the SPIDER typing
        if ((*occupancyGrid)[x2][y] == NONE || (*occupancyGrid)[x2][y] == MUSHROOM)
        {
            (*occupancyGrid)[x2][y] = SPIDER;
        }


        // Reset the Spider's movement clock
        clock.restart();

        // Return the types of the potential collisions
        return collisionType;
    }
    else
    {
        // Smooth motion on screen
        
        // If the spider is active on screen, smooth its on-screen motion
        if (isActive == true)
        {
            // Set the default y direction to the spider's current y direction
            int yDirection = movementDeltaY;
            if (y == MAIN_WINDOW_HEIGHT - 1)
            {
                // If the spider is on the bottom row, move the sprite upward
                yDirection = -1;
            }
            else if (y == MAIN_WINDOW_HEIGHT - PLAYER_MOVEMENT_AREA_HEIGHT)
            {
                // If the spider is at the top of the player movement area, move it downward
                yDirection = 1;
            }

            // Update the sprite's position based on prorated movement between the actual backend movement steps between every SPIDER_MS_PER_MOVE ms
            int xOffset = (nextMovementX * SPRITE_WIDTH * clock.getElapsedTime().asMilliseconds()) / SPIDER_MS_PER_MOVE;
            int yOffset = (yDirection * SPRITE_HEIGHT * clock.getElapsedTime().asMilliseconds()) / SPIDER_MS_PER_MOVE;
            spriteSpider.setPosition(getScreenXFromGrid(x1) + xOffset, getScreenYFromGrid(y) + yOffset);
            
        }

        // Since the spider is not actually moving on the grid, no new collisions should be handled (return default [NONE, NONE]). This will only occur on normal movement steps
        return collisionType;
    }
    
}


// Purpose: Activate the spider
void Spider::activate()
{
    // If there was nothing else besides a Mushroom in the occuancyGrid cell, put the left side of the Spider there
    // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the SPIDER typing
    if ((*occupancyGrid)[x1][y] == NONE || (*occupancyGrid)[x1][y] == MUSHROOM)
    {
        (*occupancyGrid)[x1][y] = SPIDER;
    }

    // If there was nothing else besides a Mushroom in the occuancyGrid cell, put the left side of the Spider there
    // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the SPIDER typing
    if ((*occupancyGrid)[x2][y] == NONE || (*occupancyGrid)[x2][y] == MUSHROOM)
    {
        (*occupancyGrid)[x2][y] = SPIDER;
    }

    // Set the spider to active
    isActive = true;
}

// Purpose: Deactivate the spider
void Spider::deactivate()
{
    isActive = false;
}


// Purpose: Getter for the sprite variable
// @return Sprite * -- pointer to the Spider's sprite
Sprite * Spider::getSprite()
{
    return &spriteSpider;
}


// Purpose: Getter for the x1 variable
// @return int -- the x1 coordinate of the Spider
int Spider::getX1()
{
    return x1;
}

// Purpose: Getter for the x2 variable
// @return int -- the x2 coordinate of the Spider
int Spider::getX2()
{
    return x2;
}

// Purpose: Getter for the y variable
// @return int -- the y coordinate of the Spider
int Spider::getY()
{
    return y;
}

// Purpose: Getter for the isActive variable
// @return int -- the current isActive state of the Spider
bool Spider::getIsActive()
{
    return isActive;
}
