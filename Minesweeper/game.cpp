#define MINE_CHAR '#'

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>

struct COORDINATE {
	int x;
	int y;
};
struct BOARD{
	struct COORDINATE size;
	struct COORDINATE cursor_pos;
	int mine_num;
	int mark_num;
	char **map; // '0'~'8': Number of adjacent mines, '*': mine
	int **status; // 0: closed, 1: marked, 2: opened
};
struct GAME {
	struct BOARD board;
	int status; // 0: continue game, 1: lost, 2: win, 3: quit order from user, 4: restart order from user
	unsigned int start_time;
	unsigned int end_time;
};

GAME game; // This will store all the game data.

int start_game(int x, int y, int mine_num);
BOARD create_board(int x, int y, int mine_num);
void open_cell(int x, int y);
void draw_screen();
int check_board();
void end_game();
void free_mem();
void gotoxy(int x, int y);

int start_game(int x, int y, int mine_num) {
	// This function will return -1 if lost or cancelled, time in second returned if won.

	COORDINATE *cursor_pos;
	COORDINATE size;
	int *status;
	char input;
	int result_time;

	// Clear screen.
	system("cls");

	// Hide cursor.
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);

	// Initialization
	game.board = create_board(x, y, mine_num);
	game.status = 0; // Continue game state.
	game.start_time = (unsigned int)time(NULL);
	size = game.board.size;
	draw_screen();

	while (1) {
		if (game.status != 0) break;

		cursor_pos = &(game.board.cursor_pos);
		status = &(game.board.status[cursor_pos->y - 1][cursor_pos->x - 1]);

		// Key input handling
		input = _getch();
		switch (input)
		{
			// Up
		case 'i':
		case 'I':
			if (cursor_pos->y == 1) {
				cursor_pos->y = size.y;
				break;
			}
			cursor_pos->y--;
			break;

			// Down
		case 'k':
		case 'K':
			if (cursor_pos->y == size.y) {
				cursor_pos->y = 1;
				break;
			}
			cursor_pos->y++;
			break;

			// Left
		case 'j':
		case 'J':
			if (cursor_pos->x == 1) {
				cursor_pos->x = size.x;
				break;
			}
			cursor_pos->x--;
			break;

			// Right
		case 'l':
		case 'L':
			if (cursor_pos->x == size.x) {
				cursor_pos->x = 1;
				break;
			}
			cursor_pos->x++;
			break;

			// Mark
		case 'o':
		case 'O':
			if (*status == 0) {
				*status = 1;
				game.board.mark_num++;
			}
			else if (*status == 1) {
				*status = 0;
				game.board.mark_num--;
			}
			break;

			// Open
		case 'u':
		case 'U':
			if (*status != 1) open_cell(cursor_pos->x, cursor_pos->y); // Do not allow to open the cell if marked.
			game.status = check_board();
			break;

			// Quit
		case 'q':
		case 'Q':
			game.status = 3;
		}
		draw_screen();
	}
	if (game.status == 3) return -1;

	end_game();
	// Set color.
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 240);
	gotoxy(game.board.size.x * 2 + 5, 4);
	if (game.status == 2) {
		printf("\aYou won the game!");
		result_time = game.end_time - game.start_time;
	}
	else if (game.status == 1) {
		printf("\aYou lost the game...");
		result_time = -1;
	}
	gotoxy(game.board.size.x * 2 + 5, 5);
	printf("Press Q to exit.");
	// Reset color.
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	while (1) {
		input = _getch();
		if (input == 'q' || input == 'Q') break;
	}

	free_mem();

	return result_time;
}



BOARD create_board(int x, int y, int mine_num) {
	// Create board with given size.

	int i, j;
	BOARD board;

	// Set size of the board.
	board.size.x = x;
	board.size.y = y;

	// Set initial cursor position.
	board.cursor_pos.x = 1;
	board.cursor_pos.y = 1;

	// Set amount of mines.
	board.mine_num = mine_num;

	// Initialize number of marks.
	board.mark_num = 0;

	// Dynamic allocation of map and status array.
	board.map = (char**)malloc(sizeof(char*)* y);
	for (i = 0; i < x; i++) board.map[i] = (char*)malloc(sizeof(char)* x);
	board.status = (int**)malloc(sizeof(int*)* y);
	for (i = 0; i < x; i++) board.status[i] = (int*)malloc(sizeof(int)* x);

	// Initialize map array as '0'.
	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++) {
			board.map[i][j] = '0';
		}
	}

	// Randomly put mines on the board.
	int index, dx, dy;
	srand((unsigned int)time(NULL));
	for (i = 0; i < mine_num; i++) {
		index = rand() % (x * y);
		if (*(*(board.map + index / y) + index % y) == MINE_CHAR) {
			i--;
			continue;
		}
		*(*(board.map + index / y) + index % y) = MINE_CHAR;

		// Add 1 to adjacent cells.
		char *adjacent_cell;
		for (dy = -1; dy <= 1; dy++) {
			for (dx = -1; dx <= 1; dx++) {
				adjacent_cell = *(board.map + index / y + dy) + index % y + dx;
				if ((dx != 0 || dy != 0) // Not the mine cell itself.
					&& (index / y + dy >= 0 && index / y + dy < y) // Prevent out of bound.
					&& (index % y + dx >= 0 && index % y + dx < x)) { // Prevent out of bound.
					if (*adjacent_cell != MINE_CHAR) { // Prevent messing up other mines.
						*adjacent_cell += 1;
					}
				}
			}
		}
	}

	// Initialize status array.
	for (i = 0; i < y; i++) {
		for (j = 0; j < x; j++) {
			board.status[i][j] = 0;
		}
	}

	return board;
}

void open_cell(int x, int y) {
	BOARD board = game.board;
	int dx, dy;
	int new_x, new_y;
	int *status = &(board.status[y - 1][x - 1]);

	if (*status != 2) {
		*status = 2;

		// Open adjacent cells if there's no mine nearby.
		if (board.map[y - 1][x - 1] == '0') {
			for (dy = -1; dy <= 1; dy++) {
				for (dx = -1; dx <= 1; dx++) {
					new_x = x + dx;
					new_y = y + dy;
					if ((new_x >= 1 && new_x <= board.size.x) && // Prevent out of bound on x.
						(new_y >= 1 && new_y <= board.size.y) && // Prevent out of bound on y.
						(dx != 0 || dy != 0)) { // Not the current cell itself.
						open_cell(new_x, new_y);
					}
				}
			}
		}
	}
}

void draw_screen() {
	// Print given game onto the screen.

	gotoxy(0, 0); // Overwrite the screen. This can prevent flickering unlike system("cls");.

	BOARD board = game.board;
	int row, col;
	int index = 0;
	int x = board.size.x, y = board.size.y;
	char board_entry;
	int status;
	unsigned int time_spent;

	for (row = 0; row < y + 2; row++) {
		for (col = 0; col < (x * 2) + 3; col++) {
			if (row == 0) { // Draw border on the first row.
				if (col == 0) printf("+");
				else if (col == ((x * 2) + 3) - 1) printf("+\n");
				else printf("-");
			}
			else if (row == (y + 2) - 1) { // Draw border on the last row.
				if (col == 0) printf("+");
				else if (col == ((x * 2) + 3) - 1) printf("+\n");
				else printf("-");
			}
			else { // Draw other rows.
				if (col == 0) printf("|"); // Draw border on the first column.
				else if (col == ((x * 2) + 3) - 1) printf("|\n"); // Draw border on the last column.

				// Mark currently selected position.
				else if (row == board.cursor_pos.y && col == ((board.cursor_pos.x - 1) * 2) + 1) printf("[");
				else if (row == board.cursor_pos.y && col == ((board.cursor_pos.x - 1) * 2) + 3) printf("]");

				else if (col % 2 == 1) printf(" ");

				// Actual board is printed by this code.
				else {
					board_entry = *(*(board.map + index / y) + index % y);
					status = *(*(board.status + index / y) + index % y);
					switch (status) {
					case 0: // Closed
						printf("-");
						break;

					case 1: // Markded
						printf("X");
						break;

					case 2: // Opened
						if (board_entry == '0') printf(" ");
						else {
							// Color the text.
							if (board_entry >= '1' && board_entry <= '5') SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board_entry - 39);
							else if (board_entry >= '6' && board_entry <= '8') SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), board_entry - 44);
							else if (board_entry == MINE_CHAR) {
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0);
								SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 192);
							}

							printf("%c", board_entry);
							// Reset color.
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
						}
						break;

					default:
						break;
					}
					index++;
				}
			}
		}
	}

	// Print info.
	time_spent = (unsigned int)time(NULL) - game.start_time;
	gotoxy(col + 2, 0);
	printf("====== GAME INFO ======");
	gotoxy(col + 2, 1);
	printf("Position: %d, %d           ", game.board.cursor_pos.x, game.board.cursor_pos.y); // Extra spaces are for preventing afterimage.
	gotoxy(col + 2, 2);
	printf("Mines: %d     ", game.board.mine_num - game.board.mark_num);
	gotoxy(col + 2, 3);
	printf("Time: %02d:%02d    ", time_spent / 60, time_spent - (time_spent / 60 * 60)); // It won't update in realtime, but it's close enough.
	gotoxy(0, row + 1);
	printf("I, K, J, L for up, down, left and right.\n");
	printf("U to open, O to mark.\n");
	printf("Q to quit.\n");
}

int check_board() {
	// Return value:
	// 0: need to continue game, 1: lost, 2: win

	int i, j;
	bool unopend_cell_exist = false;
	BOARD board = game.board;

	for (i = 0; i < board.size.y; i++) {
		for (j = 0; j < board.size.x; j++) {
			// Check if opened mine cell, or only mine cells are left or game has to be continued.
			if (board.map[i][j] == MINE_CHAR && board.status[i][j] == 2) return 1; // Mine cell is opend -> lost
			if (board.map[i][j] != MINE_CHAR && board.status[i][j] == 0) unopend_cell_exist = true;
		}
	}

	if (unopend_cell_exist) return 0;
	return 2;
}

void end_game() {
	int i, j;

	// Show all the mines.
	for (i = 0; i < game.board.size.y; i++) {
		for (j = 0; j < game.board.size.x; j++) {
			if (game.board.map[i][j] == MINE_CHAR) game.board.status[i][j] = 2;
		}
	}
	draw_screen();

	// Save end time.
	game.end_time = (unsigned int)time(NULL);
}

void free_mem() {
	BOARD board = game.board;
	int i;

	// Map array
	for (i = 0; i < board.size.x; i++) free(board.map[i]);
	free(board.map);

	// Status array
	for (i = 0; i < board.size.x; i++) free(board.status[i]);
	free(board.status);
}

void gotoxy(int x, int y)
{
	COORD Pos = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}