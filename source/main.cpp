#include "game.h"

const bool debugMode = true;
Game game;

void RenderGame()
{
    game.Render();
}

int main(void)
{
    // Prepare graphics
    game.Prepare3DGraphics();
    game.Prepare2DGraphics();

    // Start the game
    game.StartMenuScreen(debugMode);

    // Run the game loop
    while (true)
    {
        // Tick the game clock
        game.Tick();

        // Process input, update game state
        game.Update();

        // Render the game 3D scene
        NE_Process(RenderGame);
    }

    return 0;
}