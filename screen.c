#include <stdio.h>
#include <pthread.h> // スレッド使用
#include <unistd.h>  // sleep使用
#include "tetris_game.h"

// 足りない関数 clear_fieldとdraw_boardとdraw_current_block

void *print_screen(void *ptr)
{
    for (;;)
    {
        printf("\033[1;1H"); // 画面の上塗り
		
    	pthread_mutex_lock(&block_mutex);
    	render_field();
    	pthread_mutex_unlock(&block_mutex);
        
    	for (int i = 0; i < ROW; i++)
        {
            printf("%s\n", field[i]);
        }
		printf("\033[%d;1H", ROW + 2);
		printf("SCORE: %d\n", score);
		usleep(90000);
    }
    return NULL;
}

void render_field()
{
    clear_field(); // 空白+壁
	draw_board(); // boardの固定ブロック
    draw_current_block(); // 落下中のブロック
}

void clear_field()
{
    for (int i = 0; i < BOARD_HEIGHT; i++)
    {
        memset(field[i], ' ', COL - 1); // 第1:初期化したいメモリ領域の先頭アドレス  第2:セットしたい値(1バイト単位)　第3:セットするバイト数
        field[i][COL - 1] = '\0';       // 文字列終端
        field[i][0] = '|';              // 左枠
        field[i][COL - 2] = '|';        // 右枠
    }
    memset(field[ROW - 1], '-', COL - 1);
    field[ROW - 1][COL - 1] = '\0';
}

void draw_board()
{
    for (int i = 0; i < BOARD_HEIGHT; i++)
    {
    	for (int j = 0; j < BOARD_WIDTH; j++)
    	{
    		if(board[i][j] != 0)
    		{
    			field[i][j + 1] = SYMBOL;
    		}
    	}
    }
}

void draw_current_block()
{
    for (int i = 0; i < 4; i++)
    {
        int x = current_block->px + current_block->x[i];
        int y = current_block->py + current_block->y[i];

        // 範囲チェック（超重要）
        if (y >= 0 && y < BOARD_HEIGHT &&
            x >= 0 && x < BOARD_WIDTH)
        {
        	// printf("\033[%d;%dH%s%c%s",y + 1, x + 2, current_block->color, SYMBOL,RESET);
            field[y][x + 1] = SYMBOL;
        }
    }
}
