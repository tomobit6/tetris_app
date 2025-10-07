#include <stdio.h>
#include <stdlib.h> // system使用
#include <string.h> // memset使用
#include <unistd.h> // sleep使用
#include <time.h>   // time使用
#include "tetris_game.h"

int board[10][10];
char field[ROW][COL];

Block blocks[BLOCK_TYPE_COUNT] = {
    // ブロックセット構造体を作って持たせる方法もある
    {{0, -1, 1, 2}, {0, 0, 0, 0}, 5, 0, 0, 0, '+'}, // I
    {{0, 1, 0, 1}, {0, 0, 1, 1}, 5, 0, 1, 0, '+'},  // O
    {{0, -1, 1, 0}, {0, 0, 0, 1}, 5, 0, 2, 0, '+'}, // T
    {{0, 1, -1, 0}, {0, 0, 1, 1}, 5, 0, 3, 0, '+'}, // S
    {{0, -1, 1, 0}, {0, 0, 1, 1}, 5, 0, 4, 0, '+'}, // Z
    {{0, -1, 1, 1}, {0, 0, 0, 1}, 5, 0, 5, 0, '+'}, // J
    {{0, 1, 2, 0}, {0, 0, 0, 1}, 5, 0, 6, 0, '+'},  // L
};
// 必ず、基点から配列要素を組み立てる必要有。また、0行目から組み立て必要。
Block block; // 作業用の空のブロック(ゲームに登場したブロック格納用)  これは他のファイルでは使わない。
Block *current_block = &block;
volatile InputType input_flag = INPUT_NONE;
int block_shape;
int block_fixed = 0;

pthread_mutex_t block_mutex = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
    pthread_t input_key_thread;
    //  pthread_t screen_thread;
    init_game();

    pthread_create(&input_key_thread, NULL, detect_input, NULL);

    for (;;)
    {
        block_shape = rand() % BLOCK_TYPE_COUNT;
        init_block(current_block, &blocks[block_shape]);

        pthread_mutex_lock(&block_mutex); // ロック
        block_fixed = 0;
        pthread_mutex_unlock(&block_mutex); // ロック解除

        while (1)
        {
            // 前回位置削除
            clear_block(current_block);

            // 入力処理
            handle_input();

            // 衝突判定
            if (is_collision(current_block))
            {
                fix_block(current_block);
                draw_block(current_block);
                print_screen();
                pthread_mutex_lock(&block_mutex); // ロック
                block_fixed = 1;
                pthread_mutex_unlock(&block_mutex); // ロック解除
                break;
            }

            // 自然落下
            current_block->py++;

            // 描画
            draw_block(current_block);
            print_screen();
        }
    }
    return 0;
}