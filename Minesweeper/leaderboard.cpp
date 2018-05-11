#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <conio.h>

struct ENTRY {
	bool exist;
	char initial[4];
	int time;
};

ENTRY leaderboard[3][10];

bool check_availability(int difficulty, int time);
void new_entry(int diffiiculty, int time);
void print_leaderboard();
void read_file();
void write_file();

bool check_availability(int difficulty, int time) {
	// Difficulty - 0: easy, 1: medium, 2: hard

	int i;

	read_file();

	for (i = 0; i < 10; i++) if (leaderboard[difficulty][i].exist == false) return true;
	if (leaderboard[difficulty][9].time > time) return true;
	return false;
}

void new_entry(int difficulty, int time) {
	int i;
	char input;
	char initial[4];
	ENTRY entry;

	system("cls");
	printf("Please enter your initial (3 alphabets): ");

	for (i = 0; i < 3; i++) {
		do {
			input = _getch();
		} while (!((input >= 65 && input <= 90) || (input >= 97 && input <= 122)));
		if (input >= 97 && input <= 122) input -= 32;
		printf("%c", input);
		initial[i] = input;
	}
	initial[3] = '\0'; // Null character at the end of the string.

	while(_getch() != 13); // Wait for the enter button.

	strcpy(entry.initial, initial);
	entry.time = time;
	entry.exist = true;

	read_file();

	for (i = 8; i >= -1; i--) {
		if (i == -1) leaderboard[difficulty][0] = entry;
		else leaderboard[difficulty][i + 1] = leaderboard[difficulty][i];
		if (leaderboard[difficulty][i].exist == false);
		else if (leaderboard[difficulty][i].time <= entry.time) {
			leaderboard[difficulty][i + 1] = entry;
			break;
		}
	}
	write_file();
}

void print_leaderboard() {
	int i, j;

	read_file();
	system("cls");
	printf("===== Leaderboard =====\n");
	printf(" EASY          MEDIUM        HARD\n");
	for (i = 0; i < 10; i++) {
		for (j = 0; j < 3; j++) {
			if (leaderboard[j][i].exist) printf("%2d %3s %02d:%02d  ", i + 1, leaderboard[j][i].initial, leaderboard[j][i].time / 60, leaderboard[j][i].time - (leaderboard[j][i].time / 60 * 60));
			else printf("              ");
		}
		printf("\n");
	}
	_getch();
}

void read_file() {
	int i, j;
	FILE *fp;
	char line_buffer[20];
	char *parsed;
	char initial[4];
	int difficulty, order, time;

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 10; j++) {
			leaderboard[i][j].exist = false;
		}
	}

	fp = fopen("leaderboard.csv", "r");
	if (fp) { // do nothing if file doesn't exist.
		while (!feof(fp)) {
			fscanf(fp, "%s\n", line_buffer);
			parsed = strtok(line_buffer, ",");
			i = 0;
			while (parsed != NULL) {
				switch (i) {
				case 0:
					difficulty = atoi(parsed);
					break;
				case 1:
					order = atoi(parsed);
					break;
				case 2:
					strcpy(initial, parsed);
					break;
				case 3:
					time = atoi(parsed);
					break;
				default:
					break;
				}
				parsed = strtok(NULL, ",");
				i++;
			}
			strcpy(leaderboard[difficulty][order].initial, initial);
			leaderboard[difficulty][order].time = time;
			leaderboard[difficulty][order].exist = true;
		}
	fclose(fp);
	}
}

void write_file() {
	int i, j;

	FILE *fp;
	fp = fopen("leaderboard.csv", "w");
	if (fp == NULL) {
		printf("\nFailed to write file.\n");
		_getch();
		return;
	}
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 10; j++) {
			if (leaderboard[i][j].exist == true) {
				fprintf(fp, "%d,%d,%s,%d\n", i, j, leaderboard[i][j].initial, leaderboard[i][j].time); // Difficulty, order, initial, time
			}
		}
	}

	fclose(fp);
}