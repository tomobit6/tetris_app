#include "tetris_game.h"

// 変数定義
int score = 0;
int high_scores[3] = {0};

void init_game()
{
    memset(board, 0, sizeof(board)); // boardの0初期化
    score = 0;
}

void init_block(Block *dest, Block *src)
{
    memcpy(dest, src, sizeof(Block)); // 形コピー

    dest->px = 5;  // 初期X（中央付近）
    dest->py = 0;  // 上から出現
    dest->rot = 0; // 回転状態リセット
}

int can_move(Block *block, int dx, int dy)
{
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i] + dx;
        int y = block->py + block->y[i] + dy;

        if (x < 0 || x >= BOARD_WIDTH ||
            y >= BOARD_HEIGHT ||
            (y >= 0 && board[y][x] != 0)) // board[y][x] != 0でブロックがあると動けない処理も含まれている
        {
            return 0; // 移動できない
        }
    }
    return 1; // 移動できる
}

int update_game(int *fall_timer, int *lock_delay)
{
    // 衝突判定
    if (is_collision(current_block))
    {
        *lock_delay += 10000; // 0.01秒

        if (*lock_delay >= LOCK_DELAY) // 衝突後固定までの猶予 衝突判定してから後19回横移動のキーを受け取るループ回る
        {
            fixed_block(current_block);

            pthread_mutex_lock(&block_mutex); // ロック
            block_fixed = 1;
            pthread_mutex_unlock(&block_mutex); // ロック解除

            usleep(50000);
            clear_full_lines();
            usleep(50000);
            return 1;
        }
    }
    else
    {
        *lock_delay = 0;
    }
    if (*fall_timer >= FALL_SPEED)
    {
        // 自然落下
        if (can_move(current_block, 0, 1))
        {
            current_block->py++;
        }
        *fall_timer = 0;
    }
    usleep(10000);
    *fall_timer += 10000;

    return 0;
}

int is_collision(Block *block)
{ // 構造体はデータが大きいのでポインタで渡す
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i];
        int y = block->py + block->y[i] + 1; // 次の位置

        if (y >= BOARD_HEIGHT || (y >= 0 && board[y][x] != 0))
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
        if (y >= 0) // T型ブロックや回転によってはy = -1があり得る。配列外要素へのアクセスを防ぐ
        {
            board[y][x] = block->type + 1; // 固定　Iはboardに1として保存。Oはboardに2として保存。etc... 空は0で保存されている。
        }
    }
    pthread_mutex_unlock(&block_mutex); // ロック解除
}

void clear_full_lines()
{
    int lines = 0;
    pthread_mutex_lock(&block_mutex);           // ロック
    for (int i = BOARD_HEIGHT - 1; i >= 0; i--) // 列を1段下げるのは下から見るのが重要
    {
        int full = 1;
        for (int j = 0; j < BOARD_WIDTH; j++)
        {
            if (board[i][j] == 0)
            {
                full = 0;
                break;
            }
        }
        if (full)
        {
            lines++;
            // 一段ずつ下にずらして削除対象の列から上が下にずれてくる。つまり列の削除処理必要なし
            for (int k = i; k > 0; k--)
            {
                memcpy(board[k], board[k - 1], sizeof(board[k]));
            }
            // 1番上をコピーし終えたら、以下で１番上を空にする。その他の行はコピーで消える。
            memset(board[0], 0, sizeof(board[0]));
            i++; // これで2段続けて1行すべて埋まっている場合でも、問題なし。(元の9行目フル消える→元の8行目が9行目に落ちるi++でまた9行目見て消える)
        }
    }
    // 点数計算
    switch (lines)
    {
    case 1:
        score += 100;
        break;
    case 2:
        score += 300;
        break;
    case 3:
        score += 500;
        break;
    case 4:
        score += 800;
        break;
    }
    pthread_mutex_unlock(&block_mutex); // ロック解除
}

void update_ranking(int score)
{
    if (score > high_scores[0])
    {
        high_scores[2] = high_scores[1];
        high_scores[1] = high_scores[0];
        high_scores[0] = score;
    }
    else if (score > high_scores[1])
    {
        high_scores[2] = high_scores[1];
        high_scores[1] = score;
    }
    else if (score > high_scores[2])
    {
        high_scores[2] = score;
    }
}