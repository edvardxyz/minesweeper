#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <time.h>

void initGameSize();

#define bombCount 99
#define gameSize 24
#define boardTotalSize 26

// compile
// gcc -std=c17 main.c -IC:\Users\ebel.skp\Documents\Projekter\Cstuff\Minesweeper\SDL2\include -LC:\Users\ebel.skp\Documents\Projekter\Cstuff\Minesweeper\SDL2\lib -Wall -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -o main

typedef struct
{
    bool edge;
    bool show;
    bool bomb;
    bool flagged;
    int adjBombs;
} cell;

void FloodFill(cell gameBoard[boardTotalSize][boardTotalSize], int clickrow, int clickcol)
{

    // if (gameBoard[clickrow][clickcol].adjBombs > 0)
    // {
    //     gameBoard[clickrow - 1][clickcol - 1].show = true;
    //     gameBoard[clickrow + 1][clickcol + 1].show = true;
    //     gameBoard[clickrow - 1][clickcol + 1].show = true;
    //     gameBoard[clickrow + 1][clickcol - 1].show = true;
    // }

    if (gameBoard[clickrow][clickcol].adjBombs > 0 || gameBoard[clickrow][clickcol].edge == true || gameBoard[clickrow][clickcol].show == true)
    {
        gameBoard[clickrow][clickcol].show = true;
        return;
    }

    gameBoard[clickrow][clickcol].show = true;

    FloodFill(gameBoard, clickrow - 1, clickcol);
    FloodFill(gameBoard, clickrow + 1, clickcol);
    FloodFill(gameBoard, clickrow, clickcol - 1);
    FloodFill(gameBoard, clickrow, clickcol + 1);


    return;
}

void printshit(cell gameBoard[boardTotalSize][boardTotalSize])
{
    printf("\n");
    printf("\n");
    for (int row = 0; row < boardTotalSize; row++)
    {
        for (int col = 0; col < boardTotalSize; col++)
        {
            printf("%d ", gameBoard[row][col].show);
        }
        printf("\n");
    }
}

void generateBoard(cell gameBoard[boardTotalSize][boardTotalSize], int clickrow, int clickcol)
{
    srand(time(NULL));

    int i = 0;
    while (i < bombCount)
    {
        int row;
        int col;
    // goto invalidbomb if bomb is placed around the clicked cell
    invalidbomb:
        // + 1 to not put bomb at border cells
        row = 1 + rand() % gameSize;
        col = 1 + rand() % gameSize;

        for (int y = -1; y < 2; y++)
        {
            for (int x = -1; x < 2; x++)
            {
                if (row == clickrow + y && col == clickcol + x)
                {
                    goto invalidbomb;
                }
            }
        }

        if (gameBoard[row][col].bomb != true)
        {
            gameBoard[row][col].bomb = true;
            i++;
            // printf("row: %d col: %d\n", row, col);
        }
    }

    int count = 0;
    int countcells = 0;
    for (int row = 1; row <= gameSize; row++)
    {
        for (int col = 1; col <= gameSize; col++)
        {

            int adjCount = 0;

            // search adjecent cells for bombs
            for (int y = -1; y < 2; y++)
            {
                for (int x = -1; x < 2; x++)
                {
                    if (y == 0 && x == 0) // skip own cell
                        continue;

                    if (gameBoard[row + y][col + x].bomb == true)
                        adjCount++;
                }
            }

            gameBoard[row][col].edge = false;
            gameBoard[row][col].adjBombs = adjCount;

            // printf("adjCount is: %d\n", adjCount);

            if (gameBoard[row][col].bomb == true)
            {
                count++;
            }
            countcells++;
        }
    }
    printf("bomb count is: %d \nAnd cell count is %d", count, countcells);
}

int main(int argc, char *argv[])
{
    // initGameSize();
    if (TTF_Init() < 0)
    {
        printf("Error: TTF_Init() failed: %s\n", SDL_GetError());
        exit(1);
    }

    // create 2d array of struct cell
    // gamesize is +2 so that a border is created around the game for the search of adjacent bombs
    // will be easier to create and not go outside array

    // int boardTotalSize = gameSize + 2;

    cell gameBoard[boardTotalSize][boardTotalSize];
    for (int row = 0; row < boardTotalSize; row++)
    {
        for (int col = 0; col < boardTotalSize; col++)
        {
            gameBoard[row][col].edge = true;
            gameBoard[row][col].adjBombs = 0;
            gameBoard[row][col].bomb = false;
            gameBoard[row][col].flagged = false;
            gameBoard[row][col].show = false;
        }
    }

    // cell **gameBoard = (cell **)calloc(boardTotalSize, sizeof(cell *));
    // for (int i = 0; i < boardTotalSize; i++)
    // {
    //     gameBoard[i] = (cell *)calloc(boardTotalSize, sizeof(cell));

    //     // for(int j = 0; j < boardTotalSize; j++){
    //     //     gameBoard[i][j];

    //     // }
    // }
    // if (gameBoard == NULL)
    // {
    //     printf("Memory allocation failure.\n");
    //     exit(1);
    // }

    int grid_cell_size = 36;
    int grid_width = gameSize;
    int grid_height = gameSize;

    // + 1 so last grid lines fit in the screen.
    int window_width = (grid_width * grid_cell_size) + 1;
    int window_height = (grid_height * grid_cell_size) + 1;

    // Dark theme.
    // SDL_Color grid_background = {22, 22, 22, 255}; // Barely Black
    SDL_Color grid_line_color = {44, 44, 44, 255}; // Dark grey
    // SDL_Color grid_cursor_ghost_color = {44, 44, 44, 255};
    SDL_Color text_color = {255, 255, 255, 255}; // White

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Initialize SDL: %s",
                     SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window;
    SDL_SetWindowTitle(window, "SDL2 Minesweeper in C");
    SDL_Renderer *renderer;

    if (SDL_CreateWindowAndRenderer(window_width, window_height, 0, &window,
                                    &renderer) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Create window and renderer: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    TTF_Font *font = TTF_OpenFont("arial.ttf", 36);
    char charDigits[][2] = {"0", "1", "2", "3", "4", "5", "6", "7", "8"};
    SDL_Rect rectDigit;
    SDL_Texture *textChars[9];

    for (int i = 0; i < 9; i++)
    {
        SDL_Surface *text = TTF_RenderText_Solid(font, charDigits[i], text_color);
        textChars[i] = SDL_CreateTextureFromSurface(renderer, text);

        SDL_FreeSurface(text);
    }

    rectDigit.w = grid_width;
    rectDigit.h = grid_height;

    SDL_bool quit = SDL_FALSE;
    SDL_Event event;

    bool leftB;
    bool rightB;
    int clickcol = 0;
    int clickrow = 0;
    bool first = true;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            leftB = false;
            rightB = false;

            switch (event.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                // rectDigit.x = (event.motion.x / grid_cell_size) * grid_cell_size;
                // printf("\n%d", (event.motion.x / grid_cell_size));
                // rectDigit.y = (event.motion.y / grid_cell_size) * grid_cell_size;
                // printf("\n%d", (event.motion.y / grid_cell_size));

                clickcol = event.motion.x / grid_cell_size + 1;
                clickrow = event.motion.y / grid_cell_size + 1;

                if (first == true)
                {
                    generateBoard(gameBoard, clickrow, clickcol);
                    FloodFill(gameBoard, clickrow, clickcol);

                    printshit(gameBoard);
                    first = false;
                }

                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    leftB = true;
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    rightB = true;
                }

                break;
            case SDL_QUIT:
                quit = SDL_TRUE;
                break;
            }
        }

        // Draw grid lines.
        SDL_SetRenderDrawColor(renderer, grid_line_color.r, grid_line_color.g,
                               grid_line_color.b, grid_line_color.a);

        for (int x = 0; x < 1 + grid_width * grid_cell_size;
             x += grid_cell_size)
        {
            SDL_RenderDrawLine(renderer, x, 0, x, window_height);
        }

        for (int y = 0; y < 1 + grid_height * grid_cell_size;
             y += grid_cell_size)
        {
            SDL_RenderDrawLine(renderer, 0, y, window_width, y);
        }

        for (int row = 1; row <= gameSize; row++)
        {
            for (int col = 1; col <= gameSize; col++)
            {
                if(gameBoard[row][col].show == true && gameBoard[row][col].bomb == false){

                    rectDigit.x = grid_cell_size * (col - 1) + grid_cell_size / 5;
                    rectDigit.y = grid_cell_size * (row - 1) + grid_cell_size / 5;
                    SDL_RenderCopy(renderer, textChars[gameBoard[row][col].adjBombs], NULL, &rectDigit);

                }

            }
        }


        // SDL_RenderCopy(renderer, textChars[0], NULL, &rectDigit);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    printf("Bomb count is: %d \nGame size is %d", bombCount, gameSize);

    // for (int i = 0; i < boardTotalSize ; i++){
    //     free(gameBoard[i]);
    // }
    // free(gameBoard);

    for (int i = 0; i < 9; i++)
    {
        SDL_DestroyTexture(textChars[i]);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

// void initGameSize(){

//     int selection = 0;
//     int sizeArray[] = {0,10,18,24};
//     int bombArray[] = {0,10,40,99};

//     do
//     {
//         printf("Select size: \n");
//         printf("1) Easy\n");
//         printf("2) Normal\n");
//         printf("3) Hard\n");
//         fscanf(stdin, "%d", &selection);
//     } while (selection < 1 || selection > 3);

//     gameSize = sizeArray[selection];
//     bombCount = bombArray[selection];
// }

// Flood-fill (node):
//  1. If node is inside set the node and return.
//  2. Set the node
//  3. Perform Flood-fill one step to the south of node.
//  4. Perform Flood-fill one step to the north of node
//  5. Perform Flood-fill one step to the west of node
//  6. Perform Flood-fill one step to the east of node
//  7. Return.

// if (row == 0 && col == 0)
// {
//     // dont look up or left
// }
// else if (row == 0 && col == gameSize - 1)
// {
//     // dont look up or right
// }
// else if (row == gameSize - 1 && col == 0)
// {
//     // dont look down or left
// }
// else if (row == gameSize - 1 && col == gameSize - 1)
// {
//     // dont look right or down
// }
// else if (row == 0)
// {
//     // dont look up
// }
// else if (row == gameSize - 1)
// {
//     // dont look down
// }
// else if (col == 0)
// {
//     // dont look left
// }
// else if (col == gameSize - 1)
// {
//     // dont look right
// }
// else
// {
//     // look all around the cell
// }

// // Draw grid background.
// SDL_SetRenderDrawColor(renderer, grid_background.r, grid_background.g,
//                        grid_background.b, grid_background.a);
// SDL_RenderClear(renderer);
