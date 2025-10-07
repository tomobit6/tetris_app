#include <stdio.h>
#include <conio.h>  // kbhit,getch使用
#include <unistd.h> // sleep使用
#include "tetris_game.h"

void *detect_input(void *ptr)
{
    for (;;)
    {
        if (kbhit())
        {
            int key = getch();

            pthread_mutex_lock(&block_mutex); // ロック
            if (key == ' ')
            {
                if (!block_fixed)
                {
                    clear_block(current_block);  // 先に消さないと、回転前の+が残る
                    rotate_block(current_block); // current_block自体はポインタ変数。*でポインタが指し示す値
                }
            }
            else
            {
                if (key == 72 || key == 80 || key == 75 || key == 77)
                {
                    if (!block_fixed)
                    {
                        clear_block(current_block); // 先に消さないと、移動前の+が残る
                        switch (key)
                        {
                        case 80:                                  // 下
                            while (can_move(current_block, 0, 1)) // 限界まで落とす
                                current_block->py++;
                            break;
                        case 75: // 左
                            if (can_move(current_block, -1, 0))
                                current_block->px--;
                            break;
                        case 77: // 右
                            if (can_move(current_block, 1, 0))
                                current_block->px++;
                            break;
                        default:
                        }
                    }
                }
            }
            pthread_mutex_unlock(&block_mutex); // ロック解除
        }
        usleep(10000);
    }
    return NULL;
}

void rotate_block(Block *block)
{
    for (int i = 0; i < 4; i++)
    {
        int old_x = block->x[i];
        int old_y = block->y[i];
        block->x[i] = -old_y;
        block->y[i] = old_x;
    }
    // 時計回り(90度回転)  x' = -y    y' = x
    // 反時計回り(90度回転) x' = y    x' = -x

    block->rot = (block->rot + 1) % 4;
}

int can_move(Block *block, int dx, int dy)
{
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i] + dx;
        int y = block->py + block->y[i] + dy;

        if (x < 1 || x >= 11 || y < 0 || y >= 10 || board[y][x] != 0) // board[y][x] != 0でブロックがあると動けない処理も含まれている
        {
            return 0; // 移動できない
        }
    }
    return 1; // 移動できる
}