#include <stdio.h>
#include <stdlib.h> // system使用 rand,srand使用
#include <string.h> // memset,memcpy使用
#include <time.h>   // time使用
#include "tetris_game.h"

void init_game()
{
    srand(time(0));
    system(" cls ");

    memset(board, 0, sizeof(board)); // boardの0初期化

    for (int i = 0; i < ROW - 1; i++)
    {
        memset(field[i], ' ', COL - 1); // 第1:初期化したいメモリ領域の先頭アドレス  第2:セットしたい値(1バイト単位)　第3:セットするバイト数
        field[i][COL - 1] = '\0';       // 文字列終端
        field[i][0] = '|';              // 左枠
        field[i][COL - 2] = '|';        // 右枠
    }
    memset(field[ROW - 1], '-', COL - 1);
    field[ROW - 1][COL - 1] = '\0';
}

void init_block(Block *dest, Block *src) // dest = destination：コピー先　　src = source：コピー元
{
    memcpy(dest, src, sizeof(Block)); // memcpy：メモリ内容をコピーする。配列や構造体をまとめてコピーしたい時に使う。
    // この3行は構造体配列で初期値定義しているので、必要ないが、後々出現位置を変える場合もあるため、残す
    dest->px = 5;
    dest->py = 0;
    dest->rot = 0;
}

int is_collision(Block *block)
{ // 構造体はデータが大きいのでポインタで渡す
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i];
        int y = block->py + block->y[i] + 1; // 次の位置

        if (y >= ROW - 1 || board[y][x] != 0)
        {
            return 1; // 衝突あり
        }
    }
    return 0; // 衝突なし
}

void fix_block(Block *block)
{
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i];
        int y = block->py + block->y[i];
        board[y][x] = 1; // 固定
    }
}