#include <stdio.h>
#include <pthread.h> // スレッド使用
#include <unistd.h>  // sleep使用
#include "tetris_game.h"

void *print_screen(void *ptr)
{
    for (;;)
    {
        printf("\033[1;1H"); // 画面の上塗り

        pthread_mutex_lock(&block_mutex); // ロック
        for (int i = 0; i < ROW; i++)
        {
            printf("%s\n", field[i]);
        }
        pthread_mutex_unlock(&block_mutex); // ロック解除

        usleep(90000);
    }
    return NULL;
}
