/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the implementation of the Centipede and its actions
*/ 

#include "ECE_Centipede.hpp"

// Purpose: Constructor
// @param occupancyGrid containing a pointer to the 2D array detailing what entity is within what grid box
ECE_Centipede::ECE_Centipede(OccupancyGrid * occupancyGrid)
{
    this->occupancyGrid = occupancyGrid;

    reset();
}

// Purpose: Sets the centipede to its default starting state
void ECE_Centipede::reset()
{
    // Set first coorddinates
    this->x = MAIN_WINDOW_WIDTH - 1 - CENTIPEDE_MAX_LENGTH;
    this->y = INFO_AREA_HEIGHT - 1;

    // Set the number of segments
    this->aliveSegments = CENTIPEDE_MAX_LENGTH;

    // Set the initial movement directions
    this->movementDeltaX = -1;
    this->movementDeltaY = 1;

    // Load in the 2 textures
    this->textureCentipedeHeadLeft.loadFromFile("graphics/CentipedeHeadLeft.png");
    this->textureCentipedeHeadRight.loadFromFile("graphics/CentipedeHeadRight.png");
    this->textureCentipedeBody.loadFromFile("graphics/CentipedeBody.png");

    // Set the initial values for the first segment (the initial head)
    segments[0].x = this->x;
    segments[0].y = this->y;
    segments[0].movementDeltaX = this->movementDeltaX;
    segments[0].movementDeltaY = this->movementDeltaY;
    segments[0].prevX = segments[0].x;
    segments[0].prevY = segments[0].y;
    segments[0].prevMovementDeltaX = segments[0].movementDeltaX;
    segments[0].prevMovementDeltaY = segments[0].movementDeltaY;
    segments[0].isHead = true;
    segments[0].spriteCentipede.setTexture(textureCentipedeHeadLeft);
    segments[0].spriteCentipede.setPosition(getScreenXFromGrid(segments[0].x), getScreenYFromGrid(segments[0].y));
    segments[0].clock.restart(); // start the head's movement timeout clock
    (*occupancyGrid)[segments[0].x][segments[0].y] = CENTIPEDE; // Put the head on the occupancyGrid

    // Set the initial values for the rest of the body segments
    for (int i = 1; i < CENTIPEDE_MAX_LENGTH; ++i)
    {
        segments[i].x = this->x + i;  // Body segment i is i grid units behind the head
        segments[i].y = this->y;      // All centipede segments are on the same line to start
        segments[i].movementDeltaX = this->movementDeltaX;  // All centipede segments follow the same movement pattern to begin with
        segments[i].movementDeltaY = this->movementDeltaY;
        segments[i].prevX = segments[i].x;
        segments[i].prevY = segments[i].y;
        segments[i].prevMovementDeltaX = segments[i].movementDeltaX;
        segments[i].prevMovementDeltaY = segments[i].movementDeltaY;
        segments[i].isHead = false;   // Mark all other segments as body (not head) segments
        segments[i].spriteCentipede.setTexture(textureCentipedeBody);
        segments[i].spriteCentipede.setPosition(getScreenXFromGrid(segments[i].x), getScreenYFromGrid(segments[i].y));
        segments[i].clock.restart();
        (*occupancyGrid)[segments[i].x][segments[i].y] = CENTIPEDE; // Put each segment on the occupancyGrid
    }
}

// Purpose: Draw all centipede segments on the window
// @param window -- a pointer to the game window to display the centipede on
void ECE_Centipede::draw(RenderWindow * window)
{
    // Loop through all segments
    for (int i = 0; i < CENTIPEDE_MAX_LENGTH; ++i)
    {
        // If the segment is still alive (non-zero movement delta), draw it
        if (segments[i].movementDeltaX != 0)
        {
            window->draw(segments[i].spriteCentipede);
        }
    }
}

// Purpose: Damage the centipede, remove the hit segment, and split the centipede based on the grid coordinates of where the centipede has been hit
// @param x -- the x coordinate of where the centipede was hit
// @param y -- the y coordinate of where the centipede was hit
// @return int containing the score gained from hitting the centipede
int ECE_Centipede::damage(int x, int y)
{
    int hitIndex = -1;
    // Loop through the entire centipede to check for what segment has been hit
    for (int i = 0; i < CENTIPEDE_MAX_LENGTH; ++i)
    {
        // If the current segment has the same coordinates as the hit coordinates, it is the hit segment
        if (segments[i].x == x && segments[i].y == y)
        {
            hitIndex = i;
            break;
        }
    }

    // If no segment matches the hit coordinates, no hit verified, so take no action. No points earned.
    if (hitIndex == -1)
    {
        return 0;
    }

    // Since there is a confirmed hit, there is 1 fewer segment still alive
    --aliveSegments;

    // Mark the segment as dead by changing its movement to 0, removing it from the occupancy grid, and setting its position to (-1, -1)
    segments[hitIndex].movementDeltaX = 0;
    segments[hitIndex].movementDeltaY = 0;
    (*occupancyGrid)[segments[hitIndex].x][segments[hitIndex].y] = NONE;
    segments[hitIndex].x = -1;
    segments[hitIndex].y = -1;

    // If the hit segment is not the tail and the next segment is not dead, split the centipede
    if (hitIndex < CENTIPEDE_MAX_LENGTH - 1)
    {
        if (segments[hitIndex + 1].movementDeltaX != 0)
        {
            // Set the next segment's texture to the correct head orientation based on movement direction
            if(segments[hitIndex + 1].movementDeltaX < 0)
            {
                segments[hitIndex + 1].spriteCentipede.setTexture(textureCentipedeHeadLeft);
            }
            else
            {
                segments[hitIndex + 1].spriteCentipede.setTexture(textureCentipedeHeadRight);
            }

            // Mark the next segment as a head
            segments[hitIndex + 1].isHead = true;
        }
    }

    // return the score gained by hitting the segment based on if it was a head or not
    return (segments[hitIndex].isHead == true) ? SCORE_CENTIPEDE_HEAD : SCORE_CENTIPEDE_BODY;

}

// Purpose: Move a segment of the centipede by 1 grid unit
// @param segmentIndex -- the index of the segment to move
// @return ObjectType -- the occupancyGrid entry for the space the segment is moving into prior to its moving (what, if anything, the segment is colliding with)
ObjectType ECE_Centipede::move(int segmentIndex)
{
    // If there has been at least CENTPEDE_MS_PER_MOVE milliseconds since the last time the segment moved, move the segment
    if (segments[segmentIndex].clock.getElapsedTime().asMilliseconds() >= CENTIPEDE_MS_PER_MOVE)
    {
        // If the segment is dead, do not move it
        if (segments[segmentIndex].movementDeltaX == 0)
        {
            // If the segment is dead, no need to do any more calculation
            return NONE;
        }

        // If the centipede segment is present on the occupancyGrid, remove it
        if ((*occupancyGrid)[segments[segmentIndex].x][segments[segmentIndex].y] == CENTIPEDE)
        {
            (*occupancyGrid)[segments[segmentIndex].x][segments[segmentIndex].y] = NONE;
        }
        
        // Set previous variables for all body segments to follow
        segments[segmentIndex].prevX = segments[segmentIndex].x;
        segments[segmentIndex].prevY = segments[segmentIndex].y;
        segments[segmentIndex].prevMovementDeltaX = segments[segmentIndex].movementDeltaX;
        segments[segmentIndex].prevMovementDeltaY = segments[segmentIndex].movementDeltaY;
        
        if (segments[segmentIndex].isHead == true)
        {

            // If the segment is at the top row of the player movement area, ensure its y movement is downwards (the original y movement)
            if (segments[segmentIndex].y == MAIN_WINDOW_HEIGHT - PLAYER_MOVEMENT_AREA_HEIGHT)
            {
                // Ensure segment is going the default direction when pass the top of the player area either from above or from below after 
                segments[segmentIndex].movementDeltaY = movementDeltaY;
            }
            
            // If the segment hits a wall or a mushroom horizontally, turn it around and move it 1 unit along the vertical axis
            if ((segments[segmentIndex].x == 0 && segments[segmentIndex].movementDeltaX < 0) || (segments[segmentIndex].x == MAIN_WINDOW_WIDTH - 1 && segments[segmentIndex].movementDeltaX > 0) 
                || (*occupancyGrid)[segments[segmentIndex].x + segments[segmentIndex].movementDeltaX][segments[segmentIndex].y] == MUSHROOM
                || (*occupancyGrid)[segments[segmentIndex].x + segments[segmentIndex].movementDeltaX][segments[segmentIndex].y] == CENTIPEDE)
            {
                // Reverse its horizontal movement direction
                segments[segmentIndex].movementDeltaX *= -1;

                // If the segment is in a bottom corner, reverse its vertical direction
                if (segments[segmentIndex].y == MAIN_WINDOW_HEIGHT - 1)
                {
                    // If the segment is in one of the bottom corners, reverse direction and go up
                    segments[segmentIndex].movementDeltaY *= -1;

                    // Break off tail to go other way
                    // Find tail
                    int tailIndex = segmentIndex;
                    while (tailIndex < CENTIPEDE_MAX_LENGTH - 1)
                    {
                        // Segment is tail if next segment is dead or a head
                        if (segments[tailIndex + 1].movementDeltaX == 0 || segments[tailIndex + 1].isHead == true)
                        {
                            break;
                        }
                        else
                        {
                            ++tailIndex;
                        }
                    }

                    // If the centipede is not 1 segment long, break off tail as a new head
                    if (tailIndex > segmentIndex)
                    {
                        // Make the segment a head
                        segments[tailIndex].isHead = true;

                        // Turn the segment around
                        segments[tailIndex].movementDeltaX *= -1;
                        
                        // Update its sprite
                        if(segments[tailIndex].movementDeltaX < 0)
                        {
                            segments[tailIndex].spriteCentipede.setTexture(textureCentipedeHeadLeft);
                        }
                        else
                        {
                            segments[tailIndex].spriteCentipede.setTexture(textureCentipedeHeadRight);
                        }
                    }
                }

                // If the segment is a head, reverse its texture's orientation to match its new direction
                if (segments[segmentIndex].isHead == true)
                {
                    if(segments[segmentIndex].movementDeltaX < 0)
                    {
                        segments[segmentIndex].spriteCentipede.setTexture(textureCentipedeHeadLeft);
                    }
                    else
                    {
                        segments[segmentIndex].spriteCentipede.setTexture(textureCentipedeHeadRight);
                    }
                }

                // Move the centipede segment 1 unit along the vertical axis
                segments[segmentIndex].y += segments[segmentIndex].movementDeltaY;
            }
            else
            {
                // Move the segment in the horizontal axis
                segments[segmentIndex].x += segments[segmentIndex].movementDeltaX;
            }
        }
        else // Centipede body segment
        {
            // Move body segments so that they follow the head

            // Set current values based on the previous segment
            // NOTE: the segment before a non-head (body) segment will always be alive else the segment would be a head
            // NOTE: segments[0] will always be a head
            segments[segmentIndex].x = segments[segmentIndex - 1].prevX;
            segments[segmentIndex].y = segments[segmentIndex - 1].prevY;
            segments[segmentIndex].movementDeltaX = segments[segmentIndex - 1].prevMovementDeltaX;
            segments[segmentIndex].movementDeltaY = segments[segmentIndex - 1].prevMovementDeltaY;
        }

        // NOTE: Sprite position is updated every cycle that the backend movement does not take place, so no need to update here

        // Save the type of the object in the space the centipede has moved into
        ObjectType collisionType = (*occupancyGrid)[segments[segmentIndex].x][segments[segmentIndex].y];

        // If there was nothing else in the occuancyGrid cell, put the centipede there
        // NOTE: If something was already in the cell, the collision handling will take care of it, so do not override it with the Centipede typing
        if ((*occupancyGrid)[segments[segmentIndex].x][segments[segmentIndex].y] == NONE)
        {
            (*occupancyGrid)[segments[segmentIndex].x][segments[segmentIndex].y] = CENTIPEDE;
        }

        // Reset the movement clock
        segments[segmentIndex].clock.restart();

        // Return the type of the potential collision
        return collisionType;
    }
    else
    {
        // Smooth motion on screen

        if (segments[segmentIndex].isHead == true)
        {
            // If the segment is at the edge of the screen, move the sprite vertically, if not, move it horizontally
            if ((segments[segmentIndex].x == 0 && segments[segmentIndex].movementDeltaX < 0) || (segments[segmentIndex].x == MAIN_WINDOW_WIDTH - 1 && segments[segmentIndex].movementDeltaX > 0) 
                || (*occupancyGrid)[segments[segmentIndex].x + segments[segmentIndex].movementDeltaX][segments[segmentIndex].y] == MUSHROOM
                || (*occupancyGrid)[segments[segmentIndex].x + segments[segmentIndex].movementDeltaX][segments[segmentIndex].y] == CENTIPEDE)
            {
                // If the segment sprite needs to move vertically, determine which direction, else, just move sprite in current horizontal direction

                // Set the default y direction to the segment's current y direction
                int yDirection = segments[segmentIndex].movementDeltaY;
                if (segments[segmentIndex].y == MAIN_WINDOW_HEIGHT - 1)
                {
                    // If the segment is on the bottom row, move the sprite the reverse of the initial y direction
                    yDirection = -1 * movementDeltaY;
                }
                else if (segments[segmentIndex].y == MAIN_WINDOW_HEIGHT - PLAYER_MOVEMENT_AREA_HEIGHT)
                {
                    // If the segment is at the top of the player movement area, move it in the initial y direction again
                    yDirection = movementDeltaY;
                }

                // Update the sprite's vertical position based on prorated movement between the actual backend movement steps between every CENTIPEDE_MS_PER_MOVE ms
                int yOffset = (yDirection * SPRITE_HEIGHT * segments[segmentIndex].clock.getElapsedTime().asMilliseconds()) / CENTIPEDE_MS_PER_MOVE;
                segments[segmentIndex].spriteCentipede.setPosition(getScreenXFromGrid(segments[segmentIndex].x), getScreenYFromGrid(segments[segmentIndex].y) + yOffset);
            
            }
            else
            {
                // Update sprite's horizontal position based on prorated movement between the actual backend movement steps between every CENTIPEDE_MS_PER_MOVE ms
                int xOffset = (segments[segmentIndex].movementDeltaX * SPRITE_WIDTH * segments[segmentIndex].clock.getElapsedTime().asMilliseconds()) / CENTIPEDE_MS_PER_MOVE;
                segments[segmentIndex].spriteCentipede.setPosition(getScreenXFromGrid(segments[segmentIndex].x) + xOffset, getScreenYFromGrid(segments[segmentIndex].y));
            }
        }
        else
        {
            // If the segment is a body segment, follow the previous segment for next movement

            // If the previous segment moved to a different row, follow it up/down. If not, move sprite in current x direction
            int yDiff = segments[segmentIndex - 1].y - segments[segmentIndex].y;
            if (yDiff != 0)
            {
                // Update the sprite's vertical position based on prorated movement between the actual backend movement steps between every CENTIPEDE_MS_PER_MOVE ms
                int yOffset = (yDiff * SPRITE_HEIGHT * segments[segmentIndex].clock.getElapsedTime().asMilliseconds()) / CENTIPEDE_MS_PER_MOVE;
                segments[segmentIndex].spriteCentipede.setPosition(getScreenXFromGrid(segments[segmentIndex].x), getScreenYFromGrid(segments[segmentIndex].y) + yOffset);
            }
            else
            {
                // Update the sprite's horizontal position based on prorated movement between the actual backend movement steps between every CENTIPEDE_MS_PER_MOVE ms
                int xOffset = (segments[segmentIndex].movementDeltaX * SPRITE_WIDTH * segments[segmentIndex].clock.getElapsedTime().asMilliseconds()) / CENTIPEDE_MS_PER_MOVE;
                segments[segmentIndex].spriteCentipede.setPosition(getScreenXFromGrid(segments[segmentIndex].x) + xOffset, getScreenYFromGrid(segments[segmentIndex].y));
            }
        }
        
        // Since the segment is not actually moving on the grid, no new collisions should be handled. This will only occur on normal movement steps
        return NONE;
    }
    
}


// Purpose: Getter for the segments variable
// @return CentipedeSegment * -- The list of centipede segments
CentipedeSegment * ECE_Centipede::getSegments()
{
    return segments;
}

// Purpose: Getter for the aliveSegments variable
// @return int -- the number of currently alive centipede segments
int ECE_Centipede::getAliveSegments()
{
    return aliveSegments;
}

// Purpose: Getter for the x variable of a given segment
// @param segment -- int containing the index of the segment to get the x coordinate of 
// @return int -- the x coordinate of the given segment
int ECE_Centipede::getSegmentX(int segment)
{
    return segments[segment].x;
}

// Purpose: Getter for the y variable of a given segment
// @param segment -- int containing the index of the segment to get the y coordinate of 
// @return int -- the y coordinate of the given segment
int ECE_Centipede::getSegmentY(int segment)
{
    return segments[segment].y;
}