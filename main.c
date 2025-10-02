#include <stdio.h>
#include <stdlib.h> // system使用
#include <string.h> // memset使用
#include <unistd.h> // sleep使用
#include "tetris_game.h"

char field[ROW][COL];

Block blocks[BLOCK_TYPE_COUNT] = {
    // ブロックセット構造体を作って持たせる方法もある
    {{0, -1, 1, 2}, {0, 0, 0, 0}, 5, 2, 0, 0, '+'}, // I
    {{0, 1, 0, 1}, {0, 0, 1, 1}, 5, 2, 1, 0, '+'},  // O
    {{0, -1, 1, 0}, {0, 0, 0, 1}, 5, 2, 2, 0, '+'}, // T
    {{0, 1, -1, 0}, {0, 0, 1, 1}, 5, 2, 3, 0, '+'}, // S
    {{0, -1, 1, 0}, {0, 0, 1, 1}, 5, 2, 4, 0, '+'}, // Z
    {{0, -1, 1, 1}, {0, 0, 0, 1}, 5, 2, 5, 0, '+'}, // J
    {{0, 1, 2, 0}, {0, 0, 0, 1}, 5, 2, 6, 0, '+'},  // L
};

int main(void)
{
    init_game();

    while (1)
    {
        for (int i = 2; i < ROW - 2; i++)
        { // ROW - 3枠線の2個上
            blocks[BLOCK_T].py = i;
            for (int j = 0; j < 4; j++)
            {
                int x = blocks[BLOCK_T].px + blocks[BLOCK_T].x[j];
                int y = blocks[BLOCK_T].py + blocks[BLOCK_T].y[j];
                field[y][x] = blocks[BLOCK_T].symbol;
            }
            print_screen();
            clear_block();
        }
    }
    return 0;
}