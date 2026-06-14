#include "tetris_game.h"

// 固定ブロック用
static const char *colors[] = { // staticはこのファイルだけが使える変数にする。いわゆるprivate変数
    CYAN, YELLOW, MAGENTA, GREEN, RED, BLUE, ORANGE};

void *print_screen(void *ptr)
{
    char screen[16384];
    for (;;)
    {
        int offset = 0;
        screen[0] = '\0'; // Cの文字列は'\0'が来るまでを文字列として扱う。１要素目のみ初期化で十分。

        // offset += sprintf(screen + offset, "\033[2J");   // 全消去
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
        switch (game_state)
        {
        case STATE_PAUSED:
            draw_pause_box(screen, &offset);
            break;
        case STATE_GAMEOVER:
            draw_gameover_box(screen, &offset);
            break;
        default:
            break;
        }
        // ちらつき防止
        printf("%s", screen); // screenにため込んだバッファを一気に描画
        fflush(stdout);
        usleep(50000); // 0.05秒 カクつき調整はここで行う。
    }
    return NULL;
}

void draw_box(char *screen, int *offset, int x, int y,
              int width, int height)
{
    // 上
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dH+", y, x);
    for (int i = 0; i < width - 2; i++)
    {
        *offset += sprintf(screen + *offset, "-");
    }
    *offset += sprintf(screen + *offset, "+");
    // 中
    for (int row = 1; row < height - 1; row++)
    {
        *offset += sprintf(screen + *offset,
                           "\033[%d;%dH|", y + row, x);
        for (int col = 0; col < width - 2; col++)
        {
            *offset += sprintf(screen + *offset, " ");
        }
        *offset += sprintf(screen + *offset, "|");
    }
    // 下
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dH+", y + height - 1, x);
    for (int i = 0; i < width - 2; i++)
    {
        *offset += sprintf(screen + *offset, "-");
    }
    *offset += sprintf(screen + *offset, "+");
}

void draw_pause_box(char *screen, int *offset)
{
    int x = 8;
    int y = 5;
    draw_box(screen, offset, x, y, 22, 7); // (x,yは座標5行目,8列目から描画)

    *offset += sprintf(screen + *offset,
                       "\033[%d;%dHPAUSED", y + 1, x + 7);

    *offset += sprintf(screen + *offset,
                       "\033[%d;%dHr : RESUME", y + 3, x + 3);

    *offset += sprintf(screen + *offset,
                       "\033[%d;%dHq : QUIT", y + 4, x + 3);
}

void draw_gameover_box(char *screen, int *offset)
{
    int x = 6;
    int y = 3;

    draw_box(screen, offset, x, y, 24, 12);
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dHGAME OVER", y + 1, x + 6);
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dHSCORE : %04d", y + 3, x + 3, score);
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dHRANKING", y + 5, x + 3);
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dH1. %04d", y + 6, x + 3, high_scores[0]);
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dH2. %04d", y + 7, x + 3, high_scores[1]);
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dH3. %04d", y + 8, x + 3, high_scores[2]);
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dHr : RETRY", y + 10, x + 3);
    *offset += sprintf(screen + *offset,
                       "\033[%d;%dHq : QUIT", y + 11, x + 3);
}

void show_cursor()
{
    printf("\033[?25h");
}

void clear_message(int row)
{
    printf("\033[%d;1H", row);
    printf("\033[K"); // 別文字を再表示する際残像が残らない
}