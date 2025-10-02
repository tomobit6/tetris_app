#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#define ROW 13 // 最後行枠用。上位2行ゲームオーバー用
#define COL 22 // 1列目及び最終列 - 1枠用。最終列はnull文字用

/*  関数のプロトタイプ宣言  */
void init_game();
void print_screen();
void clear_block();

// extern int board[24][10];    // 内部用
extern char field[ROW][COL]; // 表示用

/* 座標
       -y
　　　  ↑
-x　←　中心　→　+x
　　　  ↓
       +y
*/

typedef struct
{                // typedefはBlockという名の構造体の型が使えるようにする型定義。つまり、Blockは変数ではない。externは変数の「外部参照」を宣言するため、一緒に使えず、Blockの型を宣言して、これにexternを付けるべき。
    int x[4];    // 各ブロックの相対x座標
    int y[4];    // 各ブロックの相対y座標
    int px;      // フィールド上の相対x座標
    int py;      // フィールド上の相対y座標
    int type;    // ブロックの種類(I,O,T,S,Z,J,L)
    int rot;     // 回転状態(0,1,2,3)
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

extern Block blocks[BLOCK_TYPE_COUNT]; // 構造体の型の別名BLOCK型の配列変数blocksにenumで作成したマクロを代入

#endif