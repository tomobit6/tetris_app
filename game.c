#include <stdio.h>
#include <stdlib.h> // system使用
#include <string.h> // memset使用
#include "tetris_game.h"

void init_game()
{
    system(" cls ");

    for (int i = 0; i < ROW - 1; i++)
    {
        memset(field[i], ' ', COL - 1); // 第1:初期化したいメモリ領域の先頭アドレス  第2:セットしたい値(1バイト単位)　第3:セットするバイト数
        field[i][COL - 1] = '\0';
        if (i >= 2)
        {
            field[i][0] = '|';
            field[i][COL - 2] = '|';
        }
    }
    memset(field[ROW - 1], '-', COL - 1);
    field[ROW - 1][COL - 1] = '\0';
}