#include "tetris_game.h"

int board[BOARD_HEIGHT][BOARD_WIDTH];

Block blocks[BLOCK_TYPE_COUNT] = {
	// x座標、y座標、移動分x座標、移動分y座標、形、回転状態、表示文字
	// ブロックセット構造体を作って持たせる方法もある
	// 毎回変わるものと不変のもので構造体を2つに分けた方が、毎回読み出すときに処理が軽くなるし、バグも起きずらい。
	{{0, -1, 1, 2}, {0, 0, 0, 0}, 0, 0, BLOCK_I, 0, CYAN},	  // I
	{{0, 1, 0, 1}, {0, 0, 1, 1}, 0, 0, BLOCK_O, 0, YELLOW},	  // O
	{{0, -1, 1, 0}, {0, 0, 0, 1}, 0, 0, BLOCK_T, 0, MAGENTA}, // T
	{{0, 1, -1, 0}, {0, 0, 1, 1}, 0, 0, BLOCK_S, 0, GREEN},	  // S
	{{0, -1, 1, 0}, {0, 0, 1, 1}, 0, 0, BLOCK_Z, 0, RED},	  // Z
	{{0, -1, 1, 1}, {0, 0, 0, 1}, 0, 0, BLOCK_J, 0, BLUE},	  // J
	{{0, 1, 2, 0}, {0, 0, 0, 1}, 0, 0, BLOCK_L, 0, ORANGE},	  // L
};
// 必ず、基点から配列要素を組み立てる必要有。また、0行目から組み立て必要。
Block block; // 作業用の空のブロック(ゲームに登場したブロック格納用)  これは他のファイルでは使わない。
Block *current_block = &block;
volatile InputType input_flag = INPUT_NONE;
volatile int block_fixed = 0;
volatile GameState game_state = STATE_PLAYING;
int block_shape;

pthread_mutex_t block_mutex = PTHREAD_MUTEX_INITIALIZER; // これ１個でいいのか？

int main(void)
{
	atexit(show_cursor); // プログラム終了時に呼ぶ関数atexit(関数名) カーソル再表示
	printf("\033[?25l"); // カーソル非表示
	printf("\033[2J");	 // 画面全消去

	srand(time(0));

	init_game();
	block_shape = rand() % BLOCK_TYPE_COUNT;
	init_block(current_block, &blocks[block_shape]);

	pthread_t input_key_thread;
	pthread_t screen_thread;

	pthread_create(&input_key_thread, NULL, detect_input, NULL);
	pthread_create(&screen_thread, NULL, print_screen, NULL);

	while (1)
	{
		init_game();
		game_state = STATE_PLAYING;

		while (1)
		{
			block_shape = rand() % BLOCK_TYPE_COUNT;
			init_block(current_block, &blocks[block_shape]);

			if (!can_move(current_block, 0, 0))
			{
				game_state = STATE_GAMEOVER;
				update_ranking(score);
				break; // ゲームループ終了
			}
			pthread_mutex_lock(&block_mutex);
			block_fixed = 0;
			pthread_mutex_unlock(&block_mutex);

			int fall_timer = 0;
			int lock_delay = 0;
			int block_finished = 0;
			while (!block_finished)
			{
				// 入力処理
				handle_input();

				// ゲーム状態管理
				switch (game_state)
				{
				case STATE_PLAYING:
					block_finished = update_game(&fall_timer, &lock_delay); // 0なら落下中　1ならブロック固定完了
					break;
				case STATE_PAUSED:
					usleep(10000);
					break;
				case STATE_GAMEOVER:	// 今は使ってない。ただ強制リスタートや時間切れ。などで使用するかも。
					block_finished = 1; // ブロック処理強制終了
					break;
				}
			}
			if (game_state == STATE_GAMEOVER)
			{
				break;
			}
		}
		// ===== GAME OVER待機 =====
		while (game_state == STATE_GAMEOVER)
		{
			handle_input();
			usleep(10000);
		}
	}
	return 0;
}