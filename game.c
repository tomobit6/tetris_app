#include <stdio.h>
#include <stdlib.h> // system使用 rand,srand使用
#include <string.h> // memset,memcpy使用
#include <time.h>   // time使用
#include "tetris_game.h"

// 変数定義
int score = 0;

void init_game()
{
    srand(time(0));
    system(" cls ");

    memset(board, 0, sizeof(board)); // boardの0初期化
	clear_field();
}

void init_block(Block *dest, Block *src)
{
    memcpy(dest, src, sizeof(Block)); // 形コピー

    dest->px = 5;   // 初期X（中央付近）
    dest->py = 0;   // 上から出現
    dest->rot = 0;  // 回転状態リセット
}

void fixed_block(Block *block)
{
    pthread_mutex_lock(&block_mutex); // ロック
    for (int i = 0; i < 4; i++)
    {
        int x = block->px + block->x[i];
        int y = block->py + block->y[i];
    	if (y >= 0)
    	{
        	board[y][x] = 1; // 固定
        	field[y][x + 1] = SYMBOL;
    	}
    }
    pthread_mutex_unlock(&block_mutex); // ロック解除
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

void clear_full_lines()
{
	int lines = 0;
    pthread_mutex_lock(&block_mutex); // ロック
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
        	for(int k = i; k > 0; k--)
        	{
        		memcpy(board[k], board[k - 1], sizeof(board[k]));
        	}
        // 1番上をコピーし終えたら、以下で１番上を空にする。
        memset(board[0], 0, sizeof(board[0]));
        i++; // これで2段続けて1行すべて埋まっている場合でも、問題なし。(元の9行目フル消える→元の8行目が9行目に落ちるi++でまた9行目見て消える)
        }
    }
	// 点数計算
	switch(lines)
	{	
	case 1: score += 100; break;
	case 2: score += 300; break;
	case 3: score += 500; break;
	case 4: score += 800; break;
	}
    pthread_mutex_unlock(&block_mutex); // ロック解除
}