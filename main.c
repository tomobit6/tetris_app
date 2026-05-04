#include <stdio.h>
#include <stdlib.h> // system使用
#include <string.h> // memset使用
#include <unistd.h> // sleep使用
#include <time.h>   // time使用
#include <conio.h>  // getch使用
#include "tetris_game.h"

int board[BOARD_HEIGHT][BOARD_WIDTH];

Block blocks[BLOCK_TYPE_COUNT] = {
	// x座標、y座標、移動分x座標、移動分y座標、形、回転状態、表示文字
    // ブロックセット構造体を作って持たせる方法もある
	// 毎回変わるものと不変のもので構造体を2つに分けた方が、毎回読み出すときに処理が軽くなるし、バグも起きずらい。
    {{0, -1, 1, 2}, {0, 0, 0, 0}, 0, 0, BLOCK_I, 0, CYAN}, // I
    {{0, 1, 0, 1}, {0, 0, 1, 1}, 0, 0, BLOCK_O, 0, YELLOW},  // O
    {{0, -1, 1, 0}, {0, 0, 0, 1}, 0, 0, BLOCK_T, 0, MAGENTA}, // T
    {{0, 1, -1, 0}, {0, 0, 1, 1}, 0, 0, BLOCK_S, 0, GREEN}, // S
    {{0, -1, 1, 0}, {0, 0, 1, 1}, 0, 0, BLOCK_Z, 0, RED}, // Z
    {{0, -1, 1, 1}, {0, 0, 0, 1}, 0, 0, BLOCK_J, 0, BLUE}, // J
    {{0, 1, 2, 0}, {0, 0, 0, 1}, 0, 0, BLOCK_L, 0, ORANGE}, // L
};
// 必ず、基点から配列要素を組み立てる必要有。また、0行目から組み立て必要。
Block block; // 作業用の空のブロック(ゲームに登場したブロック格納用)  これは他のファイルでは使わない。
Block *current_block = &block;
volatile InputType input_flag = INPUT_NONE;
volatile int block_fixed = 0;
int block_shape;

pthread_mutex_t block_mutex = PTHREAD_MUTEX_INITIALIZER; // これ１個でいいのか？

int main(void)
{
	srand(time(0));
	
	init_game();
	block_shape = rand() % BLOCK_TYPE_COUNT;
    init_block(current_block, &blocks[block_shape]);
	
    pthread_t input_key_thread;
    pthread_t screen_thread;

    pthread_create(&input_key_thread, NULL, detect_input, NULL);
    pthread_create(&screen_thread, NULL, print_screen, NULL);

	while(1)
	{
		init_game();
		while(!game_over)
    	{
        	block_shape = rand() % BLOCK_TYPE_COUNT;
        	init_block(current_block, &blocks[block_shape]);
    	
    		if(!can_move(current_block, 0, 0))
			{
    			game_over = 1;
    			break; // ゲームループ終了
			}
    		pthread_mutex_lock(&block_mutex);
			block_fixed = 0;
			pthread_mutex_unlock(&block_mutex);
    	
    		int fall_timer = 0;
        	while (1)
        	{
            	// 入力処理 
            	handle_input();

            	// 衝突判定
            	if (is_collision(current_block))
            	{
                	fixed_block(current_block);
            		
                	pthread_mutex_lock(&block_mutex); // ロック
                	block_fixed = 1;
                	pthread_mutex_unlock(&block_mutex); // ロック解除
            		
                	usleep(150000);
                	clear_full_lines();
                	usleep(150000);
                	break;
            	}
        		if(fall_timer >= 400000)
        		{
            		// 自然落下
            		current_block->py++;
        			fall_timer = 0;
        		}
            	usleep(10000);
        		fall_timer += 10000;
        	}
    	}
		
		// ===== GAME OVER表示 =====
		print_game_over();
		
		// ===== 入力待ち =====
        while (1)
        {
            if (kbhit())
            {
                char c = getch();
                if (c == 'r') break;    // リトライ
                if (c == 'q') return 0; // 終了
            }
            usleep(10000);
        }
    	
	}
	return 0;
}