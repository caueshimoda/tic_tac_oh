#include <stdio.h>
#include <strings.h>
#include "TTO_structures.h"
#include "TTO_functions.h"
#include "TTO_conditions.h"


int main() {
	int last_lvl, choice, state = 0, lvls[MAX_LVL];
	last_lvl = load_save(lvls);
	printf("tic tac oh...\nby cau\210 shimoda\n\n");
	choice = menu(last_lvl);
	if (choice) {
		if (choice == 2) {
			last_lvl = 1;
			save(last_lvl);
		}
		initialize_lvls_state(lvls, last_lvl);
		BOARD board;
		choice = last_lvl;
		if (choice <= MAX_LVL) load_lvl("lvls.txt", choice, &board);
		while (last_lvl <= MAX_LVL) {
			while (!completed(board)){
				if (state > 1) {
					choice = map(last_lvl, lvls, 1);
					if (choice) {
						load_lvl("lvls.txt", choice, &board);
					}
					state = 0;
				} 
				else state = game_loop(&board, state, last_lvl);	
			}
			board.state = WON;
			draw_board(board, 0);
			printf("\n");
			if (board.lvl == last_lvl) {
				lvls[last_lvl - 1] = LVL_COMPLETED;
				lvls[last_lvl] = LVL_PLAYING;
				last_lvl++;
				save(last_lvl);
				choice = last_lvl;
			} 
			else {
				choice = map(last_lvl, lvls, 1);
				if (!choice) choice = last_lvl;
			}
			load_lvl("lvls.txt", choice, &board);
			state = 0;	
		}
		int quadrants[N_QUADRANTS], i;
		for (i = 0; i < N_QUADRANTS; i++) {
			quadrants[i] = LVL_COMPLETED;
		}
		while (1) {
			state = 0;
			choice = map(last_lvl, lvls, 1);
			if (!choice) exit(0);
			load_lvl("lvls.txt", choice, &board);
			while (!winner(board) && board.free_slots > 0){
				if (state > 1) {
					choice = map(last_lvl, lvls, 1);
					if (choice) {
						load_lvl("lvls.txt", choice, &board);
					}
					state = 0;
				} 
				else state = game_loop(&board, state, last_lvl);	
			}
			if (completed(board)) {
				board.state = WON;
				draw_board(board, 0);
				if (lvls[choice - 1] != LVL_COMPLETED) update_lvls(lvls, quadrants, choice, LVL_COMPLETED);
			}
			else {
				board.state = LOST;
				draw_board(board, 0);
				if (lvls[choice - 1] != LVL_RED) update_lvls(lvls, quadrants, choice, LVL_RED);
			}
			if (final_cond(quadrants)) break;
		}
		map(last_lvl, lvls, 0);
		printf(ANSI_BLUE);
		printf("\nTHE END\n");
		printf(RESET_TEXT_FORMAT);
		printf("\nyou solved all the puzzles, a remarkable achievement\nthank you for playing\n- cau\210");	
	}	
	return 0;
}

