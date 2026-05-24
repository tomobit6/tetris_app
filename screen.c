#include <stdio.h>
#include <pthread.h> // スレッド使用
#include <unistd.h>  // sleep使用
#include "tetris_game.h"

// 固定ブロック用
static const char *colors[] = { // staticはこのファイルだけが使える変数にする。いわゆるprivate変数
    CYAN, YELLOW, MAGENTA, GREEN, RED, BLUE, ORANGE};

void *print_screen(void *ptr)
{
    char screen[4096];
    for (;;)
    {
        int offset = 0;
        screen[0] = '\0';

        offset += sprintf(screen + offset, "\033[1;1H"); // カーソル戻す

        pthread_mutex_lock(&block_mutex);

        for (int i = 0; i < BOARD_HEIGHT; i++)
        {
            offset += sprintf(screen + offset, "|"); // screenのバッファをoffset分進めた箇所からscreenに代入。screenに"|\0"が入る。sprintfは文字数を返す。つまり1

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
                            offset += sprintf(screen + offset,
                                              "%s%c%s", // 落下中ブロック表示用　実際画面に見える文字は + の1文字だけ。
                                              current_block->color,
                                              SYMBOL,
                                              RESET);
                            drawn = 1; // このマスすでに描画済み判定用
                            break;
                        }
                    }
                }

                // 固定ブロック
                if (!drawn)
                {
                    if (board[i][j] != 0)
                    {
                        offset += sprintf(screen + offset, // 固定ブロック描画用
                                          "%s%c%s",        // 色変更、＋、色元に戻す　このRESETないと、これ以降すべてこの色になる
                                          colors[board[i][j] - 1],
                                          SYMBOL,
                                          RESET);
                    }
                    else
                    {
                        offset += sprintf(screen + offset, " "); // 空白描画用
                    }
                }
            }
            offset += sprintf(screen + offset, "|\n");
        }

        // 下壁
        for (int i = 0; i < BOARD_WIDTH + 2; i++)
            offset += sprintf(screen + offset, "-");
        offset += sprintf(screen + offset, "\n");

        offset += sprintf(screen + offset, "SCORE: %04d\n", score);
        pthread_mutex_unlock(&block_mutex);

        // ポーズ画面
        offset += sprintf(screen + offset, "\033[%d;1H", ROW + 2);
        if (paused)
        {
            offset += sprintf(screen + offset, "=== PAUSED ===");
        }
        else
        {
            offset += sprintf(screen + offset, "\033[K");
        }

        // ちらつき防止
        printf("%s", screen); // screenにため込んだバッファを一気に描画
        fflush(stdout);

        usleep(50000); // 0.05秒 カクつき調整はここで行う。
    }
    return NULL;
}

void show_cursor()
{
    printf("\033[?25h");
}

void print_game_over()
{
    clear_message(ROW + 3);
    printf("GAME OVER (r: retry / q: quit)");
}

void clear_message(int row)
{
    printf("\033[%d;1H", row);
    printf("\033[K"); // 別文字を再表示する際残像が残らない
}