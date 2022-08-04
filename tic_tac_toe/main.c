#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include <foxos/term.h>

#include <buildin/number_parser.h>

typedef struct {
	char field[3][3];
	char player;
} ttt_game_t;

ttt_game_t new_game() {
	return (ttt_game_t) {
		.field = {
			{' ', ' ', ' '},
			{' ', ' ', ' '},
			{' ', ' ', ' '}
		},
		.player = 'X'
	};
}

bool is_game_over(ttt_game_t* game, char* winner) {
	// check rows
	for (int i = 0; i < 3; i++) {
		if ((game->field[i][0] == game->field[i][1] && game->field[i][1] == game->field[i][2]) && game->field[i][0] != ' ') {
			*winner = game->field[i][0];
			return true;
		}
	}

	// check columns
	for (int i = 0; i < 3; i++) {
		if ((game->field[0][i] == game->field[1][i] && game->field[1][i] == game->field[2][i]) && game->field[0][i] != ' ') {
			*winner = game->field[0][i];
			return true;
		}
	}

	// check diagonals
	if ((game->field[0][0] == game->field[1][1] && game->field[1][1] == game->field[2][2]) && game->field[0][0] != ' ') {
		*winner = game->field[0][0];
		return true;
	}

	if ((game->field[0][2] == game->field[1][1] && game->field[1][1] == game->field[2][0]) && game->field[0][2] != ' ') {
		*winner = game->field[0][2];
		return true;
	}

	// check if there is a free field
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (game->field[i][j] == ' ') {
				return false;
			}
		}
	}

	// no winner
	*winner = '-';
	return true;
}

bool take_turn(ttt_game_t* game, char player, int x, int y) {
	if (game->field[x][y] != ' ') {
		return false;
	}

	game->field[x][y] = player;
	game->player = (game->player == 'X') ? 'O' : 'X';
	return true;
}

int minmax_min(int a, int b) {
	return (a < b) ? a : b;
}

int minmax_max(int a, int b) {
	return (a > b) ? a : b;
}

int minmax(ttt_game_t* game, char player, int depth, bool is_maximizing) {
	char winner;
	if (is_game_over(game, &winner)) {
		if (winner == 'X') {
			return -10 + depth;
		} else if (winner == 'O') {
			return 10 - depth;
		} else {
			return 0;
		}
	}

	if (is_maximizing) {
		int best = -1000;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (game->field[i][j] == ' ') {
					game->field[i][j] = player;
					best = minmax_max(best, minmax(game, (player == 'X') ? 'O' : 'X', depth + 1, !is_maximizing));
					game->field[i][j] = ' ';
				}
			}
		}
		return best;
	} else {
		int best = 1000;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (game->field[i][j] == ' ') {
					game->field[i][j] = player;
					best = minmax_min(best, minmax(game, (player == 'X') ? 'O' : 'X', depth + 1, !is_maximizing));
					game->field[i][j] = ' ';
				}
			}
		}
		return best;
	}
}

void ai_move(ttt_game_t* game) {
	assert(game->player == 'O');

	int best = -1000;
	int best_x = -1;
	int best_y = -1;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (game->field[i][j] == ' ') {
				game->field[i][j] = 'O';
				int score = minmax(game, 'X', 0, false);
				game->field[i][j] = ' ';

				if (score > best) {
					best = score;
					best_x = i;
					best_y = j;
				}
			}
		}
	}

	assert(best_x != -1 && best_y != -1);
	take_turn(game, 'O', best_x, best_y);
}

void render_board(ttt_game_t* game) {
	// printf("\n\n  ");
	// for (int i = 0; i < 3; i++) {
	// 	printf("%d ", i + 1);
	// }
	// printf(" y\n");
	// printf("\n");

	// for (int i = 0; i < 3; i++) {
	// 	printf("%d ", i + 1);
	// 	for (int j = 0; j < 3; j++) {
	// 		printf("%c ", game->field[i][j]);
	// 	}
	// 	printf("\n");
	// }
	// printf("x\n\n");

	printf("\n\n");
	printf(" %c | %c | %c \n", game->field[0][0], game->field[0][1], game->field[0][2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c \n", game->field[1][0], game->field[1][1], game->field[1][2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c \n", game->field[2][0], game->field[2][1], game->field[2][2]);
	printf("\n\n");
}

int main() {
	ttt_game_t game = new_game();

	char input_x[16] = {0};
	char input_y[16] = {0};


	while (true) {
		uint32_t old_color = get_color();
		set_color(0);
		clear_screen();
		set_color(old_color);

		printf("Player %c\n", game.player);
		render_board(&game);

	input_retry:
		if (game.player == 'X') {
			printf("Enter move x: ");
			memset(input_x, 0, sizeof(input_x));
			gets(input_x);

			printf("Enter move y: ");
			memset(input_y, 0, sizeof(input_y));
			gets(input_y);

			int x;
			__libc_parse_number(input_x, &x);
			int y;
			__libc_parse_number(input_y, &y);

			printf("move: %d %d\n", x, y);

			if (x < 1 || x > 3 || y < 1 || y > 3) {
				printf("Invalid move\n");
				goto input_retry;
			}

			if (!take_turn(&game, game.player, x - 1, y - 1)) {
				printf("Invalid move\n");
				goto input_retry;
			}
		} else {
			ai_move(&game);
		}

		char winner;
		if (is_game_over(&game, &winner)) {
			uint32_t old_color = get_color();
			set_color(0);
			clear_screen();
			set_color(old_color);

			printf("Player %c wins!\n", winner);

			render_board(&game);
			break;
		}
	}

	return 0;
}