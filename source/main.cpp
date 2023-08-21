#include "game.h"

const bool DEBUG_MODE = true;
Game game;

const char *GetSaveFileName(const int argc, const char **argv)
{
    char saveName[256] = {'\0'};
    if (argc > 0 && strlen(argv[0]) > 4)
    {
        snprintf(saveName, 255, "faT:/%.*s.sav", strlen(argv[0]) - 4, argv[0]);
    }
    else
    {
        sprintf(saveName, "fat:/breaking-bad-ds.sav");
    }
    return saveName;
}

void RenderGame()
{
    game.Render();
}

int main(const int argc, const char **argv)
{
    srand(time(NULL));

    // Load save data (todo -- disabled for now)
    game.LoadSaveFile(GetSaveFileName(argc, argv));

    // Prepare graphics
    game.Prepare3DGraphics();
    game.Prepare2DGraphics();

    // Start the game
    game.StartMenuScreen(DEBUG_MODE);

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