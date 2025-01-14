#include <stdio.h>
#include <strings.h>
#ifdef _WIN32
#include <windows.h>
#endif //_WIN32
#include "TTO_structures.h"
#include "TTO_functions.h"
#include "TTO_conditions.h"
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif


enum options {EXIT_GAME = 0, NEW_GAME = 2};


// Enables ANSI text format coding (only for Windows OS)
void enable_virtual_terminal_processing() {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;

    // Get the current console mode
    if (GetConsoleMode(hConsole, &dwMode)) {
        // Set the console mode to allow virtual terminal processing (ANSI escape codes)
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, dwMode);
    }
#endif //_WIN32
}


int main() {
    enable_virtual_terminal_processing();
	int last_lvl, choice, state = 0, lvls[MAX_LVL];
	last_lvl = load_save(lvls); //from TTO_functions.h
	printf("tic tac oh...\nby cau\210 shimoda\n\n");
	choice = menu(last_lvl); //from TTO_functions.h
	
	if (choice != EXIT_GAME) {
		if (choice == NEW_GAME) {
			last_lvl = 1;
			save(last_lvl); //from TTO_functions.h
		}
		initialize_lvls_state(lvls, last_lvl); //from TTO_functions.h
		BOARD board; //from TTO_structures.h
		choice = last_lvl;
		if (choice <= MAX_LVL) load_lvl("lvls.txt", choice, &board); //from TTO_functions.h
		
		// Main game loop: increments last_lvl when a level is completed until it surpasses MAX_LEVEL
		while (last_lvl <= MAX_LVL) {
			while (!completed(board)){ //from TTO_conditions.h
				// Call map function when player chooses to see the levels map. The "cal_menu" argument set to 1 makes it call the map menu
				if (state > 1) { // state > 1: see levels map
					choice = map(last_lvl, lvls, 1); //from TTO_functions.h
					if (choice) {
						load_lvl("lvls.txt", choice, &board); // Load picked level from levels map
					}
					state = 0;
				} 
				else state = game_loop(&board, state, last_lvl); //from TTO_functions.h	
			}
			
			// Draw board in green after a level is completed 
			// Changing the state to WON ensures the draw_board function will print it in green
			// The argument 0 says it's not a transition drawing
			board.state = WON;
			draw_board(board, 0); //from TTO_functions.h
			printf("\n");
			
			// Increment last_lvl if the players was playing the last level
			if (board.lvl == last_lvl) {
				lvls[last_lvl - 1] = LVL_COMPLETED;
				lvls[last_lvl] = LVL_PLAYING;
				last_lvl++;
				save(last_lvl);
				choice = last_lvl;
			}
			// Go back to the map if the player was playing an already completed level 
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
		
		// When all the levels are completed, there's still a secret one to solve
		while (1) {
			state = 0;
			choice = map(last_lvl, lvls, 1);
			if (!choice) exit(0);
			load_lvl("lvls.txt", choice, &board);
			
			// Game loop for picked level
			while (!winner(board) && board.free_slots > 0){ //winner(board) from TTO_conditions.h
				if (state > 1) {
					choice = map(last_lvl, lvls, 1);
					if (choice) {
						load_lvl("lvls.txt", choice, &board);
					}
					state = 0;
				} 
				else state = game_loop(&board, state, last_lvl);	
			}
			
			// Update level map quadrant with the appropriate state (LVL_COMPLETED or LVL_RED)
			if (completed(board)) {
				board.state = WON;
				draw_board(board, 0);
				if (lvls[choice - 1] != LVL_COMPLETED) update_lvls(lvls, quadrants, choice, LVL_COMPLETED); //from TTO_functions.h 
			}
			else {
				board.state = LOST;
				draw_board(board, 0);
				if (lvls[choice - 1] != LVL_RED) update_lvls(lvls, quadrants, choice, LVL_RED);
			}
			if (final_cond(quadrants)) break; //from TTO_conditions.h
		}
		
		// Draw the map one last time, with "cal_menu" argument set to 0 so it doesn't call the map menu
		map(last_lvl, lvls, 0);
		printf(ANSI_BLUE);
		printf("\nTHE END\n");
		printf(RESET_TEXT_FORMAT);
		printf("\nyou solved all the puzzles, a remarkable achievement\nthank you for playing\n- cau\210");	
	}	
	return 0;
}

