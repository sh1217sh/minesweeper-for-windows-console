/*
* Minesweeper
* SW Programming (2) project
* 2016. 11. 2. ~
*
* By: Sungho Lee
* Contact: sh1217sh@cau.ac.kr or sh1217sh@gmail.com
*
* Minesweeper game for Windows console environment.
*
* Control
* I, K, J, L for up, down, left and right.
* U to open, O to mark.
*/

#include "game.h"
#include "leaderboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdbool.h>

char menu();

int main() {
	char input = 0;
	int result_time;

	while (input != 'q' && input != 'Q') {
		result_time = -1;
		input = menu();
		switch (input) {
		case '1':
			result_time = start_game(9, 9, 10);
			break;
		case '2':
			result_time = start_game(14, 14, 35);
			break;
		case '3':
			result_time = start_game(18, 18, 85);
			break;
		case '4':
			print_leaderboard();
		default:
			break;
		}
		if (result_time != -1) {
			if (check_availability(input - 49, result_time)) new_entry(input - 49, result_time);
			print_leaderboard();
		}
	}

	return 0;
}

char menu() {
	// Clear screen.
	system("cls");

	printf("<< Mine sweeper >>\n\n");

	printf("===== Select menu =====\n");
	printf("1. Play (easy)\n");
	printf("2. Play (medium)\n");
	printf("3. Play (hard)\n");
	printf("4. Leaderboard\n");
	printf("Q. Quit\n");

	return _getch();
}