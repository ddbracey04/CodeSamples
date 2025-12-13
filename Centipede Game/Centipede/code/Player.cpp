/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the implementation of the Player/Starship and its actions
*/ 

#include "Player.hpp"

// Purpose: Constructor
// @param occupancyGrid -- a pointer to the 2D array detailing what entity is within what grid box
Player::Player(OccupancyGrid * occupancyGrid)
{
    // Save pointer to occupancy grid to class variable
    this->occupancyGrid = occupancyGrid;

    // Reset the player to set up default values
    reset(MAX_PLAYER_HEALTH);
}

// Purpose: Reset the player object to its default state with a given health
// @param health -- The remaining health of the player after reset
void Player::reset(int health)
{
    // Set the default player coordinates
    this->x = MAIN_WINDOW_WIDTH / 2;
    this->y = MAIN_WINDOW_HEIGHT - 1;

    // Set the player health
    this->health = health;

    // Setup the player sprite and texture
    this->texturePlayer.loadFromFile("graphics/StarShip.png");
    this->spritePlayer.setTexture(texturePlayer);
    this->spritePlayer.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y));

    // Put the player on the occupancy grid
    (*occupancyGrid)[x][y] = PLAYER;
}

// Purpose: Move the player right by 1 grid unit
// @return ObjectType -- the occupancyGrid entry for the space the player is moving into prior to its moving (what, if anything, the player is colliding with)
ObjectType Player::moveRight()
{
    // If the player is either at right edge of map or movement is blocked by a mushroom, do nothing
    if (x == MAIN_WINDOW_WIDTH - 1 || (*occupancyGrid)[x + 1][y] == MUSHROOM)
    {
        return NONE;
    } 
 
    // If the player is present on the occupancyGrid, remove it
    if ((*occupancyGrid)[x][y] == PLAYER)
    {
        (*occupancyGrid)[x][y] = NONE;
    }
    
    // Move the player right 1 grid unit
    ++x;

    // Update the player's sprite's position
    spritePlayer.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y));

    // Save the collision type based on what was in the cell before the player moved in
    ObjectType collisionType = (*occupancyGrid)[x][y];
    
    // If there was nothing else besides a Lazer Blast in the occuancyGrid cell, put the Player there
    // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the PLAYER typing
    if ((*occupancyGrid)[x][y] == NONE || (*occupancyGrid)[x][y] == LAZER)
    {
        (*occupancyGrid)[x][y] = PLAYER;
    }

    return collisionType;

}

// Purpose: Move the player left by 1 grid unit
// @return ObjectType -- the occupancyGrid entry for the space the player is moving into prior to its moving (what, if anything, the player is colliding with)
ObjectType Player::moveLeft()
{
    // If the player is either at left edge of map or movement is blocked by a mushroom, do nothing
    if (x == 0 || (*occupancyGrid)[x - 1][y] == MUSHROOM)
    {
        return NONE;
    } 

    // If the player is present on the occupancyGrid, remove it
    if ((*occupancyGrid)[x][y] == PLAYER)
    {
        (*occupancyGrid)[x][y] = NONE;
    }
    
    // Move the player left 1 grid unit
    --x;

    // Update the player's sprite's position
    spritePlayer.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y));

    // Save the collision type based on what was in the cell before the player moved in
    ObjectType collisionType = (*occupancyGrid)[x][y];
    
    // If there was nothing else besides a Lazer Blast in the occuancyGrid cell, put the Player there
    // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the PLAYER typing
    if ((*occupancyGrid)[x][y] == NONE || (*occupancyGrid)[x][y] == LAZER)
    {
        (*occupancyGrid)[x][y] = PLAYER;
    }

    return collisionType;

}

// Purpose: Move the player up by 1 grid unit
// @return ObjectType -- the occupancyGrid entry for the space the player is moving into prior to its moving (what, if anything, the player is colliding with)
ObjectType Player::moveUp()
{
    // If the player is either at upper edge of moveable area or movement is blocked by a mushroom, do nothing
    if (y == MAIN_WINDOW_HEIGHT - PLAYER_MOVEMENT_AREA_HEIGHT || (*occupancyGrid)[x][y - 1] == MUSHROOM)
    {
        return NONE;
    } 

    // If the player is present on the occupancyGrid, remove it
    if ((*occupancyGrid)[x][y] == PLAYER)
    {
        (*occupancyGrid)[x][y] = NONE;
    }

    // Move the player up 1 grid unit 
    --y;

    // Update the player's sprite's position
    spritePlayer.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y));

    // Save the collision type based on what was in the cell before the player moved in
    ObjectType collisionType = (*occupancyGrid)[x][y];
    
    // If there was nothing else besides a Lazer Blast in the occuancyGrid cell, put the Player there
    // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the PLAYER typing
    if ((*occupancyGrid)[x][y] == NONE || (*occupancyGrid)[x][y] == LAZER)
    {
        (*occupancyGrid)[x][y] = PLAYER;
    }

    return collisionType;

}

// Purpose: Move the player down by 1 grid unit
// @return ObjectType -- the occupancyGrid entry for the space the player is moving into prior to its moving (what, if anything, the player is colliding with)
ObjectType Player::moveDown()
{
    // If the player is either at bottom edge of map or movement is blocked by a mushroom, do nothing
    if (y == MAIN_WINDOW_HEIGHT - 1 || (*occupancyGrid)[x][y + 1] == MUSHROOM)
    {
        return NONE;
    } 

    // If the player is present on the occupancyGrid, remove it
    if ((*occupancyGrid)[x][y] == PLAYER)
    {
        (*occupancyGrid)[x][y] = NONE;
    }
    
    // Move the player down 1 grid unit 
    ++y;

    // Update the player's sprite's position
    spritePlayer.setPosition(getScreenXFromGrid(x), getScreenYFromGrid(y));

    // Save the collision type based on what was in the cell before the player moved in
    ObjectType collisionType = (*occupancyGrid)[x][y];
    
    // If there was nothing else besides a Lazer Blast in the occuancyGrid cell, put the Player there
    // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the PLAYER typing
    if ((*occupancyGrid)[x][y] == NONE || (*occupancyGrid)[x][y] == LAZER)
    {
        (*occupancyGrid)[x][y] = PLAYER;
    }

    return collisionType;

}

// Purpose: Damage the player by 1 health
// @return int -- The player's new health
int Player::damage()
{
    return --health;
}

// Purpose: Getter for the sprite variable
// @return Sprite * -- pointer to the Player's sprite
Sprite * Player::getSprite()
{
    return &spritePlayer;
}

// Purpose: Getter for the x variable
// @return int -- the x coordinate of the Player
int Player::getX()
{
    return x;
}

// Purpose: Getter for the y variable
// @return int -- the y coordinate of the Player
int Player::getY()
{
    return y;
}

// Purpose: Getter for the health variable
// @return int -- the current health of the player
int Player::getHealth()
{
    return health;
}