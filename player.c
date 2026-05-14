#include <stdio.h>
#include <conio.h>  // kbhit,getch使用
#include <unistd.h> // sleep使用
#include "tetris_game.h"

void *detect_input(void *ptr)
{

    for (;;)
    {
        pthread_mutex_lock(&block_mutex);
        int bf = !block_fixed;
        pthread_mutex_unlock(&block_mutex);
        if (kbhit())
        {
            int key = getch();
            if (bf)
            {
                pthread_mutex_lock(&block_mutex);
                switch (key)
                {
                case ' ':
                    input_flag = INPUT_ROTATE;
                    break;
                case 75:
                    input_flag = INPUT_LEFT;
                    break;
                case 77:
                    input_flag = INPUT_RIGHT;
                    break;
                case 80:
                    input_flag = INPUT_DOWN;
                    break;
                default:
                    break;
                }
                pthread_mutex_unlock(&block_mutex);
            }
        }
        usleep(10000);
    }
    return NULL;
}

void handle_input()
{
    pthread_mutex_lock(&block_mutex); // ロック
    switch (input_flag)
    {
    case INPUT_LEFT:
        if (can_move(current_block, -1, 0))
            current_block->px--;
        break;
    case INPUT_RIGHT:
        if (can_move(current_block, 1, 0))
            current_block->px++;
        break;
    case INPUT_DOWN:
        while (can_move(current_block, 0, 1))
            current_block->py++;
        break;
    case INPUT_ROTATE:
        rotate_block(current_block);
        break;
    default:
        break;
    }
    input_flag = INPUT_NONE;            // 処理済み
    pthread_mutex_unlock(&block_mutex); // ロック解除
}

void rotate_block(Block *block)
{
	if (block->type == BLOCK_O)
        return;
	
    for (int i = 0; i < 4; i++)
    {
        int old_x = block->x[i];
        int old_y = block->y[i];

        block->x[i] = -old_y;
        block->y[i] = old_x;
    }
	// 正常
	if (can_move(block, 0, 0)){
		block->rot = (block->rot + 1) % 4;
    	return;
	}
	
	// 左キック
	if (can_move(block, -1, 0))
	{
    	block->px--;
		block->rot = (block->rot + 1) % 4;
    	return;
	}
	
	// 右キック
	if (can_move(block, 1, 0))
	{
    	block->px++;
		block->rot = (block->rot + 1) % 4;
    	return;
	}
	
	// だめなら戻す
	rotate_block_reverse(block);
}

void rotate_block_reverse(Block *block)
{
    for (int i = 0; i < 4; i++)
    {
        int old_x = block->x[i];
        int old_y = block->y[i];
        block->x[i] = old_y;
        block->y[i] = -old_x;
    }
    block->rot = (block->rot + 3) % 4;
}

// 時計回り(90度回転)  x' = -y    y' = x
// 反時計回り(90度回転) x' = y    x' = -x
/*
	[ 0 -1 ]
	[ 1  0 ]

	これをかけると：
	x' = 0*x + (-1)*y = -y
	y' = 1*x +  0*y = x
*/
