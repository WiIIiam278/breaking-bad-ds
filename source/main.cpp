#include "game.h"

Game game;
volatile int frame = 0;

void RenderGame()
{
    game.Render(frame);
}

int main(void)
{
    // Prepare graphics
    game.Prepare3DGraphics();
    game.Prepare2DGraphics();

    // Start the game
    game.StartTitleScreen();

    // Run the game loop
    while (true)
    {
        // Increment the frame counter
        frame++;

        // Process input, update game state
        game.Update(frame);

        // Render the game 3D scene
        NE_Process(RenderGame);
    }

    return 0;
}