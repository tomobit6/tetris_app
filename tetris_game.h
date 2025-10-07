#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include <pthread.h> // mutex使用(各ファイルスレッド使用)

#define ROW 11 // 最後行枠用。上位2行ゲームオーバー用　　　　　 実際10
#define COL 13 // 1列目及び最終列 - 1枠用。最終列はnull文字用  実際10

typedef struct
{                // typedefはBlockという名の構造体の型が使えるようにする型定義。つまり、Blockは変数ではない。externは変数の「外部参照」を宣言するため、一緒に使えず、Blockの型を宣言して、これにexternを付けるべき。
    int x[4];    // 各ブロックの相対x座標
    int y[4];    // 各ブロックの相対y座標
    int px;      // フィールド上の相対x座標
    int py;      // フィールド上の相対y座標
    int type;    // ブロックの種類(I,O,T,S,Z,J,L) ※今のところ用途なし
    int rot;     // 回転状態(0,1,2,3)   0 = 0度   1 = 90度   2 = 180度   3 = 270度
    char symbol; // 描画用の記号

} Block;

typedef enum
{
    BLOCK_I,
    BLOCK_O,
    BLOCK_T,
    BLOCK_S,
    BLOCK_Z,
    BLOCK_J,
    BLOCK_L,
    BLOCK_TYPE_COUNT
} BlockType;

typedef enum
{
    INPUT_NONE,
    INPUT_LEFT,
    INPUT_RIGHT,
    INPUT_DOWN,
    INPUT_ROTATE
} InputType;

/*  関数のプロトタイプ宣言  */
void init_game();
void init_block(Block *dest, Block *src);
void print_screen();
void draw_block(Block *block);
void clear_block(Block *block);
int is_collision(Block *block);
void fix_block(Block *block);
void rotate_block(Block *block);
void rotate_block_reverse(Block *block);
int can_move(Block *block, int dx, int dy);
void handle_input();
/*  スレッド関数プロトタイプ宣言  */
void *detect_input(void *ptr);

extern Block blocks[BLOCK_TYPE_COUNT]; // 構造体の型の別名BLOCK型の配列変数blocksにenumで作成したマクロを代入
extern int board[10][10];              // 内部用　純粋なプレイ領域管理。null文字や枠線の管理不要
extern char field[ROW][COL];           // 表示用
// 表示用fieldはx = 1~10の配列でゲーム内の盤面管理
// 内部用boardはx = 0~9の配列でゲーム内の盤面管理

/* 座標
       -y
　　　  ↑
-x　←　中心　→　+x
　　　  ↓
       +y
*/
extern int block_shape;
extern Block *current_block;
extern volatile int block_fixed;
extern volatile InputType input_flag;
extern pthread_mutex_t block_mutex;

#endif