#include <stdio.h>
#include <pthread.h> // スレッド使用
#include <unistd.h>  // sleep使用
#include "tetris_game.h"

// 固定ブロック用
static const char *colors[] = {
    CYAN, YELLOW, MAGENTA, GREEN, RED, BLUE, ORANGE
};

void *print_screen(void *ptr)
{
    for (;;)
    {
        printf("\033[1;1H"); // カーソル戻す

        pthread_mutex_lock(&block_mutex);

        for (int i = 0; i < BOARD_HEIGHT; i++)
        {
            printf("|");

            for (int j = 0; j < BOARD_WIDTH; j++)
            {
                int drawn = 0;

                // 落下中ブロック優先
                if (current_block != NULL)
                {
                    for (int k = 0; k < 4; k++)
                    {
                        int x = current_block->px + current_block->x[k];
                        int y = current_block->py + current_block->y[k];

                        if (x == j && y == i)
                        {
                            printf("%s%c%s",
                                   current_block->color,
                                   SYMBOL,
                                   RESET);
                            drawn = 1;
                            break;
                        }
                    }
                }

                // 固定ブロック
                if (!drawn)
                {
                    if (board[i][j] != 0)
                    {
                        printf("%s%c%s",
                               colors[board[i][j] - 1],
                               SYMBOL,
                               RESET);
                    }
                    else
                    {
                        printf(" ");
                    }
                }
            }

            printf("|\n");
        }

        // 下壁
        for (int i = 0; i < BOARD_WIDTH + 2; i++)
            printf("-");
        printf("\n");

        printf("SCORE: %d\n", score);
    	
    	// ちらつき防止
    	fflush(stdout);

        pthread_mutex_unlock(&block_mutex);

        usleep(90000);
    }
	return NULL;
}

void print_game_over(){
    printf("\033[%d;1H", ROW + 3);
    printf("GAME OVER (r: retry / q: quit)");
}
