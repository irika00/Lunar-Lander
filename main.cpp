/**

* Author: Irika Aggarwal

* Assignment: Lunar Lander!

* Date due: 30 Oct 2020

* I pledge that I have completed this assignment without

* collaborating with anyone else, in conformance with the

* NYU School of Engineering Policies and Procedures on

* Academic Misconduct.

**/


#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1450,
              SCREEN_HEIGHT = 800,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#00004d";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr int   NUMBER_OF_TILES         = 17;
constexpr float TILE_DIMENSION          = 100.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 4.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f;



constexpr float MAX_LANDING_SPEED = 80.0f;
constexpr float OFFSCREEN_MARGIN = 30.0f;
                

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;
float gTimeAccumulator = 0.0f;


Entity *gProtag = nullptr;
Entity *gTiles = nullptr;
Entity *gWinScreen = nullptr;
Entity *gLoseScreen = nullptr;
int     gTileCount = 0;



// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Lunar Lander!");

    /*
        ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> animationAtlas = {
        { UP,    { 0,1,2 } },
        { LEFT,  {15,16,17 } },
        { RIGHT, { 6,7,8 } },
    };

    float sizeRatio = 20.0f / 64.0f;

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    gProtag = new Entity(
        { ORIGIN.x - 200.0f, 100.0f}, // position
        { 250.0f * sizeRatio, 250.0f },  // scale
        "assets/game/ship.png",          // texture file address
        ATLAS,                           // single image or atlas?
        { 8, 3 },                        // atlas dimensions
        animationAtlas                   // actual atlas
    );

    gProtag->setDirectionalColliders({ 87.0f, 150.0f }, { 50.0f, 146.0f });

    gProtag->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    gProtag->setThrust(300.0f);
    gProtag->setFuel(70.0f);
    gProtag->setBurnRate(10.0f);
    gProtag->setPhysics(true);

    /*
        ----------- TILES -----------
    */
    gTiles = new Entity[NUMBER_OF_TILES * 2];
    gTileCount = 0;

    float leftMostX =  20.0f;

    for (int i = 0; i < NUMBER_OF_TILES - 1; i++)
    {
            gTiles[i].setTexture("assets/game/block.png");
            gTiles[i].setScale({ TILE_DIMENSION, TILE_DIMENSION});
            gTiles[i].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
            gTiles[i].setPosition({
            leftMostX + i * TILE_DIMENSION,
            (ORIGIN.y) + TILE_DIMENSION + 250.0f 
            });  
            
            gTileCount++;  
        
    }
    
    int firstTopIndex = gTileCount; 

    for (int i = 0; i < NUMBER_OF_TILES; i++)
    {
        if (i % 3 == 0 || i % 5 == 0)
        {
            Entity &tile = gTiles[gTileCount];
            tile.setTexture("assets/game/block.png");
            tile.setScale({ TILE_DIMENSION, TILE_DIMENSION });
            tile.setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION });
            tile.setPosition({
                leftMostX + i * TILE_DIMENSION,
                ORIGIN.y + 250.0f
            });
            gTileCount++;
        }
    }

    int first = firstTopIndex; 
    int last  = gTileCount - 1;
    gTiles[first].setPlatformMotion(40.0f, gTiles[first].getPosition().x,
                                       gTiles[first].getPosition().x + 100.0f);
    gTiles[last ].setPlatformMotion(40.0f, gTiles[last ].getPosition().x - 100.0f,
                                        gTiles[last ].getPosition().x);

    for (int i = 0; i < gTileCount; i++)
    TraceLog(LOG_INFO, "tile %d pos=(%.0f,%.0f) col=(%.0f,%.0f)", i,
             gTiles[i].getPosition().x, gTiles[i].getPosition().y,
             gTiles[i].getColliderDimensions().x, gTiles[i].getColliderDimensions().y);

    // end screens
    gWinScreen = new Entity(
        { ORIGIN.x, ORIGIN.y }, // position
        { SCREEN_WIDTH, SCREEN_HEIGHT },  // scale
        "assets/game/win.png",          // texture file address
        SINGLE,                           // single image or atlas?
        { 0, 0 },                        // atlas dimensions
        animationAtlas                   // actual atlas
    );

    gLoseScreen = new Entity(
        { ORIGIN.x, ORIGIN.y }, // position
        { SCREEN_WIDTH, SCREEN_HEIGHT },  // scale
        "assets/game/lose.png",          // texture file address
        SINGLE,                           // single image or atlas?
        { 0, 0 },                        // atlas dimensions
        animationAtlas                   // actual atlas
    );

    SetTargetFPS(FPS);
}

void processInput()
{
    gProtag->resetAcceleration();

    if      (IsKeyDown(KEY_A)) gProtag->accelerateLeft();
    else if (IsKeyDown(KEY_D)) gProtag->accelerateRight();

    if (IsKeyDown(KEY_W)) gProtag->accelerateUp();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    while (deltaTime >= FIXED_TIMESTEP)
    {
        if (!gProtag->hasPhysics()) break;

        gTiles[16].updatePlatform(FIXED_TIMESTEP);
        gTiles[gTileCount - 1].updatePlatform(FIXED_TIMESTEP);

        gProtag->update(FIXED_TIMESTEP);
        gProtag->update(FIXED_TIMESTEP, gTiles, gTileCount);

        deltaTime -= FIXED_TIMESTEP;
    }
    gTimeAccumulator = deltaTime;

    Vector2 position = gProtag->getPosition();

    bool offScreen = position.x < -OFFSCREEN_MARGIN ||
                    position.x > SCREEN_WIDTH  + OFFSCREEN_MARGIN ||
                    position.y < -OFFSCREEN_MARGIN ||
                    position.y > SCREEN_HEIGHT + OFFSCREEN_MARGIN;

    if (offScreen)
    {
        gAppStatus = END_LOSE;
    }
    else if (!gProtag->hasPhysics())   
    {
        bool gentle = gProtag->getImpactSpeedY() < MAX_LANDING_SPEED;
        gAppStatus = (!gProtag->hasCrashed() && gentle) ? END_WIN : END_LOSE;
    }



}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gProtag->render();
    gProtag->displayCollider();
    DrawText(TextFormat("Fuel: %.0f", gProtag->getFuel()), 20, 20, 30, WHITE);

    for (int i = 0; i < gTileCount; i++) {
        gTiles[i].render();
        gTiles[i].displayCollider();
    }
    

    if (gAppStatus == END_WIN)
    {
        gWinScreen->render();

    }
    if (gAppStatus == END_LOSE)
    {
        gLoseScreen->render();

    }

    EndDrawing();
}

void shutdown()
{
    // Unload the textures while the window (GL context) still exists
    delete gProtag;
    delete[] gTiles;
    delete gLoseScreen;
    delete gWinScreen;
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING || gAppStatus == END_LOSE || gAppStatus == END_WIN)
    {
        if (gAppStatus == RUNNING)
        {
            processInput();
            update();
            render();
        }
        else
        {
            processInput();
            render();   
        }
     
    }

    shutdown();

    return 0;
}
