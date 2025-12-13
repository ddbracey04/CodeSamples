/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the declaration of the Spider and its actions
*/ 

#ifndef SPIDER_HPP
#define SPIDER_HPP

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <list>
#include <random>

using namespace sf;

// Purpose: Contain and govern all fields, states, and functions of a single Spider in the game
class Spider
{

public:
    // Purpose: Constructor
    // @param occupancyGrid -- a pointer to the 2D array detailing what entity is within what grid box
    Spider(OccupancyGrid * occupancyGrid);

    // Purpose: Reset the spider object to its default state with a given health
    void reset();

    // Purpose: Damage the spider and remove it from the occupancy grid
    void damage();

    // Purpose: Move the spider vertically or diagonally by 1 grid unit
    // @return ObjectType * -- A 2-entry array with the collision for both cells of the spider
    ObjectType * move();

    // Purpose: Activate the spider
    void activate();
    
    // Purpose: Deactivate the spider
    void deactivate();


    //////// Getters

    // Purpose: Getter for the sprite variable
    // @return Sprite * -- pointer to the Spider's sprite
    Sprite * getSprite();

    // Purpose: Getter for the x1 variable
    // @return int -- the x1 coordinate of the Spider
    int getX1();
    
    // Purpose: Getter for the x2 variable
    // @return int -- the x2 coordinate of the Spider
    int getX2();
    
    // Purpose: Getter for the y variable
    // @return int -- the y coordinate of the Spider
    int getY();
    
    // Purpose: Getter for the isActive variable
    // @return int -- the current isActive state of the Spider
    bool getIsActive();

private:


    // Pointer to the occupancyGrid for the game to reference location and determine collisions
    OccupancyGrid * occupancyGrid;

    // Spider textures and sprites
    Texture textureSpider;
    Sprite spriteSpider;

    // Movement clock
    Clock clock;

    // Respawn timeout clock
    Clock respawnClock;

    // Random generator for random spawn location and horizontal movement
    std::default_random_engine generator;

    // Array to serve as return for calls to move
    ObjectType collisionType[2] = {NONE, NONE};

    // Spider active or inactive
    bool isActive;

    // Grid coordinates for the Spider
    int x1;
    int x2;
    int y;

    // Movement direction of the Spider
    int movementDeltaX;
    int movementDeltaY;

    // Next horizontal movement of the Spider
    int nextMovementX;
};

#endif