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

    pthread_mutex_lock(&block_mutex); // ロック
    for (int i = 0; i < ROW - 1; i++)
    {
        memset(field[i], ' ', COL - 1); // 第1:初期化したいメモリ領域の先頭アドレス  第2:セットしたい値(1バイト単位)　第3:セットするバイト数
        field[i][COL - 1] = '\0';       // 文字列終端
        field[i][0] = '|';              // 左枠
        field[i][COL - 2] = '|';        // 右枠
    }
    memset(field[ROW - 1], '-', COL - 1);
    field[ROW - 1][COL - 1] = '\0';
    pthread_mutex_unlock(&block_mutex); // ロック解除
}

void init_block(Block *dest, Block *src) // dest = destination：コピー先　　src = source：コピー元
{
    memcpy(dest, src, sizeof(Block)); // memcpy：メモリ内容をコピーする。配列や構造体をまとめてコピーしたい時に使う。
    // この3行は構造体配列で初期値定義しているので、必要ないが、後々出現位置を変える場合もあるため、残す
    dest->px = 5;
    dest->py = 0; // pyを変更してから、描画してるため。-1することで最上段からブロックが落ちてくる
    dest->rot = 0;
}

void clear_block(Block *block)
{
    pthread_mutex_lock(&block_mutex); // ロック
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i];
        int y = block->py + block->y[i];
        field[y][x] = ' ';
    }
    pthread_mutex_unlock(&block_mutex); // ロック解除
}

void draw_block(Block *block)
{
    pthread_mutex_lock(&block_mutex); // ロック
    for (int i = 0; i < 4; i++)
    {
        int x = current_block->px + current_block->x[i];
        int y = current_block->py + current_block->y[i];
        field[y][x] = current_block->symbol;
    }
    pthread_mutex_unlock(&block_mutex); // ロック解除
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

void fixed_block(Block *block)
{
    pthread_mutex_lock(&block_mutex); // ロック
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i];
        int y = block->py + block->y[i];
        board[y][x] = 1; // 固定
        field[y][x] = current_block->symbol;
    }
    pthread_mutex_unlock(&block_mutex); // ロック解除
}

void clear_full_lines()
{
    pthread_mutex_lock(&block_mutex); // ロック
    for (int i = 0; i < 10; i++)
    {
        int full = 1;
        for (int j = 0; j < 10; j++)
        {
            if (board[i][j] == 0)
            {
                full = 0;
                break;
            }
        }
        if (full)
        {
            memset(board[i], 0, sizeof(board[i]));
            memset(field[i] + 1, ' ', COL - 3); // field[i] + 1はポインタの2行目field[1]を表す
        }
    }
    pthread_mutex_unlock(&block_mutex); // ロック解除
}