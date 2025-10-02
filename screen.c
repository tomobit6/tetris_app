#include <stdio.h>
#include <unistd.h> // sleep使用
#include "tetris_game.h"

void print_screen()
{
    for (int i = 0; i < ROW; i++)
    {
        printf("%s\n", field[i]);
    }
    usleep(300000);
}

void clear_block()
{
    for (int i = 0; i < 4; i++)
    {
        int x = blocks[BLOCK_T].px + blocks[BLOCK_T].x[i];
        int y = blocks[BLOCK_T].py + blocks[BLOCK_T].y[i];
        field[y][x] = ' ';
    }
}