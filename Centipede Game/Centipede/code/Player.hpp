/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the declaration of the Player/Starship and its actions
*/ 

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include "Constants.h"

using namespace sf;

// Purpose: Contain and govern all fields, states, and functions of a single Player in the game
class Player
{

public:
    // Purpose: Constructor
    // @param occupancyGrid -- a pointer to the 2D array detailing what entity is within what grid box
    Player(OccupancyGrid * occupancyGrid);


    // Purpose: Reset the player object to its default state with a given health
    // @param health -- The remaining health of the player after reset
    void reset(int health);


    // Purpose: Move the player right by 1 grid unit
    // @return ObjectType -- the occupancyGrid entry for the space the player is moving into prior to its moving (what, if anything, the player is colliding with)
    ObjectType moveRight();

    // Purpose: Move the player left by 1 grid unit
    // @return ObjectType -- the occupancyGrid entry for the space the player is moving into prior to its moving (what, if anything, the player is colliding with)
    ObjectType moveLeft();
    
    // Purpose: Move the player up by 1 grid unit
    // @return ObjectType -- the occupancyGrid entry for the space the player is moving into prior to its moving (what, if anything, the player is colliding with)
    ObjectType moveUp();
    
    // Purpose: Move the player down by 1 grid unit
    // @return ObjectType -- the occupancyGrid entry for the space the player is moving into prior to its moving (what, if anything, the player is colliding with)
    ObjectType moveDown();


    // Purpose: Damage the player by 1 health
    // @return int -- The player's new health
    int damage();


    //////// Getters

    // Purpose: Getter for the sprite variable
    // @return Sprite * -- pointer to the Player's sprite
    Sprite * getSprite();
    
    // Purpose: Getter for the x variable
    // @return int -- the x coordinate of the Player
    int getX();

    // Purpose: Getter for the y variable
    // @return int -- the y coordinate of the Player
    int getY();

    // Purpose: Getter for the health variable
    // @return int -- the current health of the player
    int getHealth();

private:

    // Pointer to the occupancyGrid for the game to reference location and determine collisions
    OccupancyGrid * occupancyGrid;

    // Player texture/sprites
    Texture texturePlayer;
    Sprite spritePlayer;

    // Grid coordinates for the Player
    int x;
    int y;

    // Current player health
    int health;
};

#endif