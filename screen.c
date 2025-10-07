#include <stdio.h>
#include <unistd.h> // sleep使用
#include "tetris_game.h"

void print_screen()
{
    printf("\033[1;1H"); // 画面の上塗り

    for (int i = 0; i < ROW; i++)
    {
        printf("%s\n", field[i]);
    }
    usleep(800000);
}

void clear_block(Block *block)
{
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i];
        int y = block->py + block->y[i];
        field[y][x] = ' ';
    }
}