/* 
Author: Dylan Bracey
Class: ECE4122 A
Last Date Modified: 9/25/24 
Description: 
This file contains the main game loop, game control functions, and game setup
*/ 

// Include important C++ libraries here
#include <SFML/Graphics.hpp>
#include <random>
#include <list>
#include <sstream>
#include <ctime>
#include "ECE_Centipede.hpp"
#include "Mushroom.hpp"
#include "Player.hpp"
#include "ECE_LazerBlast.hpp"
#include "Spider.hpp"

// Make code easier to type with "using namespace"
using namespace sf;

// Types used for defining and tracking the state of the game
typedef enum
{
    START_MENU,
    IN_GAME,
    GAME_OVER
} GameStates;

GameStates gameState;

// Create a window
sf::RenderWindow window(sf::VideoMode(800, 600), "CENTIPEDE");
sf::Texture texture;
sf::Sprite sprite;

int score;

OccupancyGrid occupancyGrid;

std::list<Mushroom> mushrooms;
ECE_Centipede centipede(&occupancyGrid);
Player player(&occupancyGrid);
std::list<ECE_LazerBlast> lazers;
Spider spider(&occupancyGrid);

Clock lazerFireTimeout;

Texture textureStarship;
Sprite remainingLives[MAX_PLAYER_HEALTH];



// Purpose: Setup the starting state from the game after transitioning from the main menu screen
void setupGame()
{
    // Clear the occupancy grid
    for (int i = 0; i < MAIN_WINDOW_WIDTH; ++i)
    {
        for (int j = 0; j < INFO_AREA_HEIGHT + GAME_AREA_HEIGHT + BOTTOM_AREA_HEIGHT; ++j)
        {
            occupancyGrid[i][j] = NONE;
        }
    }

    // Reset the score
    score = 0;

    // Switch the background texture to a solid color
    texture.loadFromFile("graphics/Background.png");

    // Clear the window of all drawn things
    window.clear();


    /////// MUSHROOMS
    // Establish a uniform_int_distribution random number generator
    std::default_random_engine generator((unsigned int) time(0));
    std::uniform_int_distribution<int> distribution(0, (MAIN_WINDOW_WIDTH * GAME_AREA_HEIGHT) - 1); // Set the range of the generator to be the number of grid blocks in the game area

    // Setup an array to track the location of all mushrooms used for ensuring mushrooms are not placed on the same square
    int placedMushrooms[NUM_MUSHROOMS];

    // Generate all mushrooms
    for (int i = 0; i < NUM_MUSHROOMS; ++i)
    {
        // Get random cell number
        placedMushrooms[i] = distribution(generator);
        bool cellFree = true;

        // Check all previously generated mushroom locations for duplicates
        for (int j = 0; j < i; ++j)
        {
            // If we are trying to place 2 mushrooms in the same place, generate a new mushroom instead
            if (placedMushrooms[i] == placedMushrooms[j])
            {
                --i;
                cellFree = false;
                break;
            }
        }
        
        // If we had duplicated a mushroom, do not add a new one to the list, regenerate it instead
        if (!cellFree)
        {
            continue;
        }

        // Create and add the new mushroom to the list
        mushrooms.emplace_front(placedMushrooms[i] % MAIN_WINDOW_WIDTH, INFO_AREA_HEIGHT + placedMushrooms[i] / MAIN_WINDOW_WIDTH);

        // Put the mushroom on the occupancy grid
        occupancyGrid[placedMushrooms[i] % MAIN_WINDOW_WIDTH][INFO_AREA_HEIGHT + placedMushrooms[i] / MAIN_WINDOW_WIDTH] = MUSHROOM;

    }


    // Reset/clear/activate all other game entities

    //////// PLAYER
    player.reset(MAX_PLAYER_HEALTH);

    //////// CENTIPEDE
    centipede.reset();

    //////// SPIDER
    spider.reset();
    spider.activate();

    //////// LAZERS
    lazers.clear();
    lazerFireTimeout.restart();


    //////// REMAINING LIVES
    // Setup the texture and position for all Remaining Life sprites
    textureStarship.loadFromFile("graphics/StarShip.png");
    for (int i = 0; i < player.getHealth(); ++i)
    {
        remainingLives[i].setTexture(textureStarship);
        remainingLives[i].setPosition(getScreenXFromGrid(MAIN_WINDOW_WIDTH - 1 - i), getScreenYFromGrid(0));
    }

    // Set the game's state to IN_GAME
    gameState = IN_GAME;
}

// Purpose: Handle the transition from the main game screen to the game over screen
void gameOver()
{
    // Clear the window of all drawn sprites and clear the mushroom list
    window.clear();
    mushrooms.clear();

    // Change the game state to GAME_OVER
    gameState = GAME_OVER;

}

// Purpose: Handle the game reset on a new game, game over, or a lost life
void resetGame()
{
    // Clear the occupancy grid
    for (int i = 0; i < MAIN_WINDOW_WIDTH; ++i)
    {
        for (int j = 0; j < INFO_AREA_HEIGHT + GAME_AREA_HEIGHT + BOTTOM_AREA_HEIGHT; ++j)
        {
            if (occupancyGrid[i][j] != MUSHROOM)
            {
                occupancyGrid[i][j] = NONE;
            }
        }
    }

    // Reset the player with the current amount of health
    player.reset(player.getHealth());

    // Reset the enemies
    centipede.reset();
    spider.reset();

    // Empty the lazer blast list and restart the firing timeout clock
    lazers.clear();
    lazerFireTimeout.restart();

}



// Purpose: Handle all damage to game entities
// @param collisionType -- The type of entity that was collided into
// @param x -- the x grid coordinate of the collision
// @param y -- the y grid coordinate of the collision
void assessDamage(ObjectType collisionType, int x, int y)
{
    int hitScore;

    // Determine what type of collision occured 
    switch (collisionType)
    {
        case MUSHROOM:  // Something ran into a mushroom
            // Find the mushroom that is in collision
            for (std::list<Mushroom>::iterator iter = mushrooms.begin(); iter != mushrooms.end(); ++iter)
            {
                // Loop through all mushrooms
                if (iter->getX() == x && iter->getY() == y)
                {    
                    // If a mushroom has the same x and y coordinate as the collision, damage the mushroom
                    if (iter->damage())
                    {
                        // If damage returns true, the mushroom is destroyed
                        iter = mushrooms.erase(iter);

                        // Remove it from the occupancy grid
                        if (occupancyGrid[x][y] == MUSHROOM)
                        {
                            occupancyGrid[x][y] = NONE;
                        }

                        // Score the mushroom
                        score += 4;
                    }

                    // If the collided mushroom was found in the mushroom list, stop iterating through it
                    break;
                }
            }
            break;

        case PLAYER: // Something ran into the player

            // If the player has no more lives remaining, game over. If he has more lives left, restart the game after damaging the player
            if (player.damage() < 0)
            {
                gameOver();
            }
            else
            {
                resetGame();
            }

            break;

        case CENTIPEDE: // Something ran into the centipede

            // Damage the centipede and store the points gained into "hitScore"
            hitScore = centipede.damage(x, y);

            // Add the points gained to the score
            score += hitScore;

            // If the centipede was actually hit (points were gained), create a mushroom at the collision site
            if (hitScore > 0)
            {
                mushrooms.emplace_front(x, y);
                occupancyGrid[x][y] = MUSHROOM;
            }

            // If there are no more remaining living centipede segments, game over
            if (centipede.getAliveSegments() == 0)
            {
                gameOver();
            }
            break;
            
        case SPIDER:
            // Destroy the spider, add the score, and deactivate the spider object
            spider.damage();
            score += SCORE_SPIDER;
            spider.deactivate();
            break;
    }
}




// Purpose: Run initial game setup and main game loop
void startGame()
{
    // Set the game state to the start menu
    gameState = START_MENU;
    
    // Load main menu background texture
    texture.loadFromFile("graphics/StartupScreenBackGround.png");

    // Create a sprite and set the texture
    sprite.setTexture(texture);

    // Define the target rectangle dimensions
    sf::FloatRect targetRect(0.f, 0.f, 800.f, 600.f); // Position (x, y) and size (width, height)

    // Get the original size of the sprite
    sf::FloatRect originalSize = sprite.getGlobalBounds();

    // Calculate scale factors
    float scaleX = targetRect.width / originalSize.width;
    float scaleY = targetRect.height / originalSize.height;

    // Set the scale of the sprite to fit the target rectangle
    sprite.setScale(scaleX, scaleY);

    // Set the position of the sprite to the target rectangle's position
    sprite.setPosition(targetRect.left, targetRect.top);


    // Create and setup the font and text objects for the in-game score and game over text
    Font font;
    Text scoreText;
    font.loadFromFile("fonts/KOMIKAP_.ttf");
    scoreText.setFont(font);
    scoreText.setString("0");
    scoreText.setCharacterSize(40);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(getScreenXFromGrid(MAIN_WINDOW_WIDTH / 2), Y_WINDOW_PADDING);

    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(40);
    gameOverText.setFillColor(Color::White);
    gameOverText.setPosition(getScreenXFromGrid(MAIN_WINDOW_WIDTH / 4), getScreenXFromGrid((MAIN_WINDOW_HEIGHT / 4) - 1));

    // Main loop
    while (window.isOpen()) 
    {
        // Event handling loop
        sf::Event event;
        while (window.pollEvent(event)) 
        {
            // If the event is closing the window, close the game window
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // Handle key presses
            if (event.type == sf::Event::KeyPressed)
            {
                // If the game state is START_MENU or GAME_OVER and the enter key is pressed, advance the game state
                if (event.key.code == sf::Keyboard::Enter && gameState != IN_GAME)
                {
                    switch (gameState)
                    {
                        case START_MENU:
                            // Start the main game
                            setupGame();
                            break;

                        case GAME_OVER:
                            // Clear the window, reset score text, reset game state to Main Menu
                            window.clear();
                            scoreText.setString("0");
                            gameState = START_MENU;
                            break;
                    }
                    
                }

                // If the left arrow or A key is pressed, move player left
                if (event.key.code == sf::Keyboard::Left || event.key.code == sf::Keyboard::A)
                {
                    ObjectType collisionType = player.moveLeft();
                    if (collisionType == CENTIPEDE || collisionType == SPIDER)
                    {
                        assessDamage(PLAYER, player.getX(), player.getY());
                    }
                }
                
                // If the right arrow or D key is pressed, move player right
                if (event.key.code == sf::Keyboard::Right || event.key.code == sf::Keyboard::D)
                {
                    ObjectType collisionType = player.moveRight();
                    if (collisionType == CENTIPEDE || collisionType == SPIDER)
                    {
                        assessDamage(PLAYER, player.getX(), player.getY());
                    }
                }
                
                // If the up arrow or W key is pressed, move player up
                if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::W)
                {
                    ObjectType collisionType = player.moveUp();
                    if (collisionType == CENTIPEDE || collisionType == SPIDER)
                    {
                        assessDamage(PLAYER, player.getX(), player.getY());
                    }
                }

                // If the down arrow or S key is pressed, move player down
                if (event.key.code == sf::Keyboard::Down || event.key.code == sf::Keyboard::S)
                {
                    ObjectType collisionType = player.moveDown();
                    if (collisionType == CENTIPEDE || collisionType == SPIDER)
                    {
                        assessDamage(PLAYER, player.getX(), player.getY());
                    }
                }

                
                // If the Spacebar is pressed, try to fire a Lazer Blast
                if (event.key.code == sf::Keyboard::Space)
                {
                    // Ensure the timeout clock has expired before creating and firing the Lazer Blast
                    if (lazerFireTimeout.getElapsedTime().asMilliseconds() >= LAZER_FIRE_TIMEOUT_MS)
                    {
                        // If enough time has passed since the last firing, create a new Lazer Blast and add it to the Lazers list
                        lazers.emplace_front(player.getX(), player.getY(), &occupancyGrid);

                        // Restart the timeout clock
                        lazerFireTimeout.restart();
                    }
                }
            }
        }

        // If the main game is in progress, move all non-player, non-mushroom entities and check for collisions
        if (gameState == IN_GAME)
        {
            // Move the lazers and check for collisions
            for (std::list<ECE_LazerBlast>::iterator iter = lazers.begin(); iter != lazers.end(); )
            {
                // If the Lazer Blast is not off-screen, move it
                if (iter->getY() >= 0)
                {
                    // Move the segment and store the collision type
                    ObjectType collisionType = iter->move();

                    // If the collision is with a damageable entity (Mushroom, Centipede, or Spider), damage it and destroy the Lazer Blast. If not, move the Lazer Blast up 1 grid unit.
                    if (collisionType == MUSHROOM || collisionType == CENTIPEDE || collisionType == SPIDER)
                    {
                        assessDamage(collisionType, iter->getX(), iter->getY());
                        iter = lazers.erase(iter);
                    }
                    else
                    {
                        ++iter;
                    }
                }
                else 
                {
                    // If the Lazer Blast is off screen, destroy it
                    iter = lazers.erase(iter);
                }
            }

            // Move each centipede segment and assess damage to Lazer or Player if necessary
            for (int i = 0; i < CENTIPEDE_MAX_LENGTH; ++i)
            {
                ObjectType collisionType = centipede.move(i);
                if (collisionType == LAZER)
                {
                    assessDamage(CENTIPEDE, centipede.getSegmentX(i), centipede.getSegmentY(i));
                }
                else if (collisionType == PLAYER)
                {
                    assessDamage(PLAYER, centipede.getSegmentX(i), centipede.getSegmentY(i));
                }
            }

            // Move the spider and assess collision damage (from both grid spaces the spider takes up) if needed
            ObjectType * collisionType = spider.move();
            if (collisionType[0] == MUSHROOM)
            {
                int curScore = score;
                // Damage mushroom 2 times to ensure it is destroyed
                assessDamage(MUSHROOM, spider.getX1(), spider.getY());
                assessDamage(MUSHROOM, spider.getX1(), spider.getY());
                score = curScore; // Adjust score back since player did not kill the mushroom
            }
            if (collisionType[1] == MUSHROOM)
            {
                int curScore = score;
                // Damage mushroom 2 times to ensure it is destroyed
                assessDamage(MUSHROOM, spider.getX2(), spider.getY());
                assessDamage(MUSHROOM, spider.getX2(), spider.getY());
                score = curScore; // Adjust score back since player did not kill the mushroom
            }
            if (collisionType[0] == LAZER || collisionType[1] == LAZER)
            {
                assessDamage(SPIDER, spider.getX1(), spider.getY());
            }
            if (collisionType[0] == PLAYER || collisionType[1] == PLAYER)
            {
                assessDamage(PLAYER, spider.getX1(), spider.getY());
            }

            // Update the displayed score
            std::stringstream ss;
            ss << score;
            scoreText.setString(ss.str());
        }

        // Clear the window of all previously drawn sprites
        window.clear(sf::Color(BACKGROUND_COLOR));

        // Draw necessary sprites to the screen based on the game state
        switch (gameState)
        {
            case START_MENU:
                // Draw the main menu background image sprite
                window.draw(sprite);
                break;
            case IN_GAME:
                // Draw the player
                window.draw(*(player.getSprite()));

                // Draw all mushrooms
                for (std::list<Mushroom>::iterator iter = mushrooms.begin(); iter != mushrooms.end(); ++iter)
                {
                    window.draw(*(iter->getSprite()));
                }

                // Draw all lazer blasts
                for (std::list<ECE_LazerBlast>::iterator iter = lazers.begin(); iter != lazers.end(); ++iter)
                {
                    window.draw(*(iter->getSprite()));
                }

                // Draw the centipede
                centipede.draw(&window);

                // Draw the spider if it is active
                if (spider.getIsActive())
                {
                    window.draw(*(spider.getSprite()));
                }

                // Draw all remaining lives
                for (int i = 0; i < player.getHealth(); ++i)
                {
                    window.draw(remainingLives[i]);
                }
                
                // Draw the score text
                window.draw(scoreText);
                
                break;
            case GAME_OVER:
                // Write and draw the Game Over screen text
                std::stringstream ss;
                ss << "GAME OVER\n\nFINAL SCORE: " << score << " \n\nPress Enter to Restart";
                gameOverText.setString(ss.str());
                window.draw(gameOverText);

                // Reset the main menu screen background
                texture.loadFromFile("graphics/StartupScreenBackGround.png");
                break;
        }

        // Render all drawn sprites
        window.display();
    }

}

// Purpose: The entrypoint into the game
// @return int -- dummy error code (return 0) to satisfy C++ requirements
int main()
{
    startGame();
    return 0;
}

