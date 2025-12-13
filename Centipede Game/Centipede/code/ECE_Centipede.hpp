/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the declaration of the Centipede and its actions
*/ 

#ifndef ECE_CENTIPEDE_HPP
#define ECE_CENTIPEDE_HPP

#include <SFML/Graphics.hpp>
#include "Constants.h"
#include <list>

using namespace sf;

// Struct containing all information for a single centipede segment
typedef struct _CentipedeSegment
{
    int x;
    int y;
    int movementDeltaX;
    int movementDeltaY;
    int prevX;
    int prevY;
    int prevMovementDeltaX;
    int prevMovementDeltaY;
    bool isHead;
    Sprite spriteCentipede;
    Clock clock;
} CentipedeSegment;

// Purpose: Contain and govern all fields, states, and functions of the centipede for the game
class ECE_Centipede
{

public:
    // Purpose: Constructor
    // @param occupancyGrid containing a pointer to the 2D array detailing what entity is within what grid box
    ECE_Centipede(OccupancyGrid * occupancyGrid);

    // Purpose: Sets the centipede to its default starting state
    void reset();

    // Purpose: Draw all centipede segments on the window
    // @param window -- a pointer to the game window to display the centipede on
    void draw(RenderWindow * window);

    // Purpose: Damage the centipede, remove the hit segment, and split the centipede based on the grid coordinates of where the centipede has been hit
    // @param x -- the x coordinate of where the centipede was hit
    // @param y -- the y coordinate of where the centipede was hit
    // @return int containing the score gained from hitting the centipede
    int damage(int x, int y);

    // Purpose: Move a segment of the centipede by 1 grid unit
    // @param segmentIndex -- the index of the segment to move
    // @return ObjectType -- the occupancyGrid entry for the space the segment is moving into prior to its moving (what, if anything, the segment is colliding with)
    ObjectType move(int i);



    //////// Getters

    // Purpose: Getter for the segments variable
    // @return CentipedeSegment * -- The list of centipede segments
    CentipedeSegment * getSegments();
    
    // Purpose: Getter for the aliveSegments variable
    // @return int -- the number of currently alive centipede segments
    int getAliveSegments();
    
    // Purpose: Getter for the x variable of a given segment
    // @param segment -- int containing the index of the segment to get the x coordinate of 
    // @return int -- the x coordinate of the given segment
    int getSegmentX(int segment);
    
    // Purpose: Getter for the y variable of a given segment
    // @param segment -- int containing the index of the segment to get the y coordinate of 
    // @return int -- the y coordinate of the given segment
    int getSegmentY(int segment);

private:

    // Pointer to the occupancyGrid for the game to reference location and determine collisions
    OccupancyGrid * occupancyGrid;  

    // Array containing all centipede segments for the game
    CentipedeSegment segments[CENTIPEDE_MAX_LENGTH];    

    // Centipede textures
    Texture textureCentipedeHeadLeft;
    Texture textureCentipedeHeadRight;
    Texture textureCentipedeBody;

    // Centipede's 1st segment (original head)'s initial coordinates
    int x;
    int y;

    // Number of alive centipede segments
    int aliveSegments;

    // Initial movement directions of the centipede
    int movementDeltaX;
    int movementDeltaY;
};

#endif