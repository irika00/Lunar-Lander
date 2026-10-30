#include "CS3113/Entity.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1450,
              SCREEN_HEIGHT = 800,
              FPS           = 120;

constexpr char    BG_COLOUR[] = "#000080";
constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr int   NUMBER_OF_TILES         = 17;
constexpr float TILE_DIMENSION          = 100.0f,
                // in m/ms², since delta time is in ms
                ACCELERATION_OF_GRAVITY = 800.0f,
                FIXED_TIMESTEP          = 1.0f / 60.0f;
                

// Global Variables
AppStatus gAppStatus = RUNNING;
float gPreviousTicks = 0.0f;

Entity *gProtag = nullptr;
Entity *gTiles  = nullptr;
Entity *gSecondTiles  = nullptr;

// Function Declarations
void initialise();
void processInput();
void update();
void render();
void shutdown();

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Physics");

    /*
        ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> animationAtlas = {
        { DOWN,  { 21,22,23 } },
        { LEFT,  {15,16,17 } },
        { UP,    { 1,1,1 } },
        { RIGHT, { 6,7,8 } },
    };

    float sizeRatio = 20.0f / 64.0f;

    // Assets from @see https://sscary.itch.io/the-adventurer-female
    gProtag = new Entity(
        { ORIGIN.x - 200.0f, ORIGIN.y - 200.0f }, // position
        { 250.0f * sizeRatio, 250.0f },  // scale
        "assets/game/ship.png",          // texture file address
        ATLAS,                           // single image or atlas?
        { 8, 3 },                        // atlas dimensions
        animationAtlas                   // actual atlas
    );

    gProtag->setColliderDimensions({
        gProtag->getScale().x / 3.0f,
        gProtag->getScale().y / 3.0f
    });

    gProtag->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
    gProtag->setThrust(300.0f);
    gProtag->setFuel(10.0f);
    gProtag->setBurnRate(10.0f);

    /*
        ----------- TILES -----------
    */
    gTiles = new Entity[NUMBER_OF_TILES];
    gSecondTiles = new Entity[NUMBER_OF_TILES];

    float leftMostX =  20.0f;//(NUMBER_OF_TILES * TILE_DIMENSION) / 2.0f ;

    for (int i = 0; i < NUMBER_OF_TILES - 1; i++)
    {
            // assets from @see https://kenney.nl/assets/pixel-platformer-industrial-expansion
            gTiles[i].setTexture("assets/game/mytilw.png");
            gTiles[i].setScale({ TILE_DIMENSION, TILE_DIMENSION});
            gTiles[i].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION});
            gTiles[i].setPosition({
            leftMostX + i * TILE_DIMENSION,
            (ORIGIN.y) + TILE_DIMENSION + 250.0f 
            });    
        
    }
    

    for (int i = 0; i < NUMBER_OF_TILES - 1; i++)
    {
            // assets from @see https://kenney.nl/assets/pixel-platformer-industrial-expansion
            gSecondTiles[i].setTexture("assets/game/mytilw.png");
            gSecondTiles[i].setScale({ TILE_DIMENSION, TILE_DIMENSION});
            gSecondTiles[i].setColliderDimensions({ TILE_DIMENSION, TILE_DIMENSION});
            gSecondTiles[i].setPosition({
            leftMostX + i * TILE_DIMENSION,
            (ORIGIN.y) + 250.0f
            });    
        
    }

    SetTargetFPS(FPS);
}

void processInput()
{
    gProtag->resetMovement();

    if      (IsKeyDown(KEY_A)) gProtag->moveLeft();
    else if (IsKeyDown(KEY_D)) gProtag->moveRight();
    
    if (IsKeyDown(KEY_W)) gProtag->moveUp();

    if (GetLength(gProtag->getMovement()) > 1.0f)
        gProtag->normaliseMovement();

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;
}

void update()
{
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    gProtag->update(deltaTime);
    gProtag->update(FIXED_TIMESTEP, gSecondTiles, NUMBER_OF_TILES);
    gProtag->setPhysics(true);

    for (int i = 0; i < NUMBER_OF_TILES; i++)
        gTiles[i].update(deltaTime);
    for (int i = 0; i < NUMBER_OF_TILES; i++)
        gSecondTiles[i].update(deltaTime);
}

void render()
{
    BeginDrawing();
    ClearBackground(ColorFromHex(BG_COLOUR));

    gProtag->render();
    DrawText(TextFormat("Fuel: %.0f", gProtag->getFuel()), 20, 20, 30, WHITE);

    for (int i = 0; i < NUMBER_OF_TILES; i++) gTiles[i].render();
    for (int i = 0; i < NUMBER_OF_TILES; i++) gSecondTiles[i].render();

    EndDrawing();
}

void shutdown()
{
    // Unload the textures while the window (GL context) still exists
    delete gProtag;
    delete[] gTiles;
    delete[] gSecondTiles;
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}
