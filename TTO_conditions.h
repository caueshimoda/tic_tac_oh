#ifndef TTO_CONDITIONS_H
#define TTO_CONDITIONS_H
#include "TTO_structures.h"


// Returns the board's winner or 0 if there's no winner
int winner(BOARD board) {
	int row, col, check = 1;
	char ref;
	
	// Check all rows for a full sequence of X's or O's
	for (row = 0; row < board.matrix; row++) {
		ref = board.placed_piece[row][0]; // sets the reference as the piece on the first column of the current row
		if (ref == 'X' || ref == 'O'){
			for (col = 1; col < board.matrix; col++) {
				if (board.placed_piece[row][col] != ref) break;
				else check += 1;
			}
			if (check == board.matrix) return ref;
			check = 1;
		}	
	}
	
	// Check all columns for a full sequence of X's or O's
	for (col = 0; col < board.matrix; col++) {
		ref = board.placed_piece[0][col]; // sets the reference as the piece on the first row of the current column
		if (ref == 'X' || ref == 'O'){
			for (row = 1; row < board.matrix; row++) {
				if (board.placed_piece[row][col] != ref) break;
				else check += 1;
			}
			if (check == board.matrix) return ref;
			check = 1;
		}	
	}
	
	// Check first diagonal, setting the reference as the piece on the first row and first column and incrementing both in the loop
	ref = board.placed_piece[0][0];
	check = 1;
	if (ref == 'X' || ref == 'O') {
		col = 1;
		for (row = 1; row < board.matrix; row++) {
			if (board.placed_piece[row][col] != ref) break;
			else check += 1;
			col++;	
		}
		if (check == board.matrix) return ref;
	}
	
	// Check second diagonal, setting the reference as the piece on the first row and last column, incrementing the row and decreasing the column in the loop
	check = 1;
	col = board.matrix - 1;
	ref = board.placed_piece[0][col];
	if (ref == 'X' || ref == 'O') {
		col--;
		for (row = 1; row < board.matrix; row++) {
			if (board.placed_piece[row][col] != ref) break;
			else check += 1;
			col--;	
		}
		if (check == board.matrix) return ref;	
	}		
	
	return 0;
}  


// Returns 1 if the standard condition to completing a level is met and 0 if it's not
int std_cond(BOARD board) {
	if (winner(board) == board.target) return 1;
	return 0;
}


// Returns 1 if the colored condition to completing a level is met and 0 if it's not
// Colored condition: all positions that are set as green slots must have a target sprite on them,
// all positions that are set as red slots must have a wrong sprite on them
int color_cond(BOARD board) {
	int i;
	
	for (i = 0; i < board.special_count; i++){
		if (board.special[i].type == GREEN_SLOT) {
			if (board.placed_piece[board.special[i].row][board.special[i].column] != board.target) return 0; 
		}
		else if (board.special[i].type == RED_SLOT) {
			if (board.placed_piece[board.special[i].row][board.special[i].column] != board.wrong) return 0; 
		} 
	}
	
	return 1;
}


// Return 1 if the summation condition to completing a level is met and 0 if it's not
// Summation condition: all rows and columns must have their respective target sums met
// Blank space = 0, O = 1, X = 2, Bomb timer = the current timer number
int summation_cond(BOARD board) {
	int row, col, sum, sum_index = 0;
	
	for (row = 0; row < board.matrix; row++) {
		sum = 0;
		for (col = 0; col < board.matrix; col++) {
			if (board.placed_piece[row][col] == 'X') sum += 2;
			else if (board.placed_piece[row][col] == 'O') sum += 1;
			else if (board.placed_piece[row][col] >= '0' && board.placed_piece[row][col] <= '9') sum += board.placed_piece[row][col] - '0';
		}
		if (sum != board.conditions.sum[sum_index]) return 0;
		sum_index ++;
	}
	
	for (col = 0; col < board.matrix; col++) {
		sum = 0;
		for (row = 0; row < board.matrix; row++) {
			if (board.placed_piece[row][col] == 'X') sum += 2;
			else if (board.placed_piece[row][col] == 'O') sum += 1;
			else if (board.placed_piece[row][col] >= '0' && board.placed_piece[row][col] <= '9') sum += board.placed_piece[row][col] - '0';
		}
		if (sum != board.conditions.sum[sum_index]) return 0;
		sum_index ++;
	}
	
	return 1;
}


// Returns 1 if all the necessary conditions for a level to be completed are met, and 0 if they're not
int completed(BOARD board) {
	
	if (board.conditions.has_colored_slot) {
		if (!color_cond(board)) return 0;	
	}
	
	if (board.conditions.summation) {
		if (!summation_cond(board)) return 0;
	}
	
	// Special condition when there's no target, and the level must end as a draw to be completed (no free slots and no winner)
	if (board.target == ' ') {
		if (!board.free_slots) {
			if (!winner(board)) return 1;
		} 
		return 0;
	} 
	
	return std_cond(board);
}


// Condition to the secret final level
int final_cond(int quadrants[N_QUADRANTS]) {
	int i, j, check_green = 0, check_red = 0;
	for (i = 0; i < N_QUADRANTS; i++) {
		if (quadrants[i] == LVL_COMPLETED) {
			check_green += 1;
			if (check_green == QUADRANT_MATRIX) return 0;	
		} 
		if (i % QUADRANT_MATRIX == QUADRANT_MATRIX - 1) {
			if (!check_green) check_red = 1;
			check_green = 0;
		}
	}
	if (check_red) return 1;
	i = 0;
	for (j = 0; j < N_QUADRANTS; j++) {
		if (quadrants[i] == LVL_COMPLETED) {
			check_green += 1;
			if (check_green == QUADRANT_MATRIX) return 0;	
		} 
		if (i + QUADRANT_MATRIX >= N_QUADRANTS) {
			if (!check_green) check_red = 1;
			check_green = 0;
			i -= QUADRANT_MATRIX * (QUADRANT_MATRIX - 1) - 1;
		}
		else i += QUADRANT_MATRIX;
	}
	if (check_red) return 1;
	i = 0;
	for (j = 0; j < QUADRANT_MATRIX; j++) {
		if (quadrants[i] == LVL_COMPLETED) {
			check_green += 1;
			if (check_green == QUADRANT_MATRIX) return 0;	
		} 
		i += QUADRANT_MATRIX + 1;
	}
	if (!check_green) check_red = 1;
	check_green = 0;
	i = QUADRANT_MATRIX - 1;
	for (j = 0; j < QUADRANT_MATRIX; j++) {
		if (quadrants[i] == LVL_COMPLETED) {
			check_green += 1;
			if (check_green == QUADRANT_MATRIX) return 0;	
		} 
		i += QUADRANT_MATRIX - 1;
	}
	if (!check_green || check_red) return 1;
	return 0;
}


#endif //TTO_CONDITIONS_H
