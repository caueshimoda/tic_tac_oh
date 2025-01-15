#ifndef TTO_FUNCTIONS_H
#define TTO_FUNCTIONS_H

#include <stdio.h>
#include "TTO_structures.h"
#include "TTO_conditions.h"
#include <strings.h>
#include <stdlib.h>

#ifndef ANSI_RED
#define ANSI_RED "\033[0;31m"
#endif //ANSI_RED
#ifndef ANSI_GREEN
#define ANSI_GREEN "\033[0;32m"
#endif //ANSI_GREEN
#ifndef ANSI_YELLOW
#define ANSI_YELLOW "\033[0;33m"
#endif //ANSI_YELLOW
#ifndef ANSI_BLUE
#define ANSI_BLUE "\033[0;34m"
#endif //ANSI_BLUE
#ifndef ANSI_UNDERLINED
#define ANSI_UNDERLINED "\033[4;39m"
#endif //ANSI_UNDERLINED
#ifndef ANSI_UYELLOW
#define ANSI_UYELLOW "\033[4;33m"
#endif //ANSI_UYELLOW
#ifndef ANSI_UBLUE
#define ANSI_UBLUE "\033[4;34m"
#endif //ANSI_UBLUE
#ifndef RESET_TEXT_FORMAT
#define RESET_TEXT_FORMAT "\033[0m"
#endif //RESET_TEXT_FORMAT


enum size {INPUT_SIZE = 3, STR_SIZE = 5, COLOR_SIZE = 5, COLUMN_DIVISOR = 3, ROW_DIVISOR = 9, BLOCK_DIVISOR = 27, MAX_LVL = 81};
enum lvl_state {PLAYING, WON, LOST};
enum matrix_index {COLUMN, ROW};
enum direction {RIGHT, LEFT};
enum sprite_color {TARGET_COLOR = 1, WRONG_COLOR, PURSUER_COLOR, PMAN_COLOR, MAGNET_COLOR, PURSUER_MAGNET_COLOR, PMAN_MAGNET_COLOR};


const char *colors[] = {
	"0",
	ANSI_GREEN,
	ANSI_RED,
	ANSI_YELLOW,
	ANSI_BLUE,
	ANSI_UNDERLINED,
	ANSI_UYELLOW,
	ANSI_UBLUE
};


// Returns the opposite piece from the received one
char opposite_sprite(char c){
	if (c == 'X') return 'O';
	return 'X';
}


// Returns the appropriate index to the *colors[] array so that the element in the given position receives the correct formatting
int sprite_color(char target, char current, SPECIAL sp[MAX_SP], int row, int col, int sp_qty) {
	int i, color = 0;
	
	// Check each special element from the special array to find if there's one with the provided coordinates
	for (i = 0; i < sp_qty; i++) {
		if (sp[i].row == row && sp[i].column == col) {
			if (sp[i].type == PURSUER) {
				if (!color) color = PURSUER_COLOR;
				else return PURSUER_MAGNET_COLOR;
			}
			else if (sp[i].type == PMAN) {
				if (!color) color = PMAN_COLOR;
				else return PMAN_MAGNET_COLOR;
			}
			else if (sp[i].type == MAGNET) {
				if (!color) color = MAGNET_COLOR;
				else {
					color += 3;
					return color;
				} 
			}
		}
	}
	
	if (color) return color;
	
	// If there was no special element in the provided position and there's a target sprite, 
	// check if the current sprite is the target or the opposite sprite and return the appropriate color
	if (target != ' ') {
		if (current == target) return TARGET_COLOR;
		if (current == opposite_sprite(target)) return WRONG_COLOR;
	}
	
	return STD_COLOR; 
}


// Returns if a given position is inside the given matrix
int in_matrix(int row, int col, int matrix) {
	if (row < 0 || row >= matrix) return 0;
	if (col < 0 || col >= matrix) return 0;
	return 1;
}


// Returns 0 if there's a special element in the given position, and 1 if there's not any
// The special elements taken into account don't include green or red slots, therefore sp[i].type < GREEN_SLOT 
int no_special(SPECIAL sp[MAX_SP], int row, int col, int qty) {
	int i;
	for (i = 0; i < qty; i++) {
		if (sp[i].type < GREEN_SLOT) {
			if (sp[i].row == row && sp[i].column == col) return 0;	
		}
	}
	return 1;
}


// Draws part of the current row from the given string, with the given color when applicable
void draw_row(char str[STR_SIZE], char c, char color[COLOR_SIZE], int state) {
	int i;
	
	// If level ended, the part drawed must be painted in green or red, depending on the state
	if (state != PLAYING){
		if (state == WON) printf(ANSI_GREEN);
		else printf(ANSI_RED);
		for (i = 0; i < STR_SIZE; i++){
			printf("%3c", str[i]);
		}
		printf(RESET_TEXT_FORMAT);
	}
	
	else {
		// Print the string using the correct color for the sprite
		int shifted = 0, comp = strcmp(color, "0");
		for (i = 0; i < STR_SIZE; i++){
			if (str[i] == c && comp != 0) { // If the color received is "0", it doesn't need to print any color
				printf(color);
				shifted = 1;
			}
			printf("%3c", str[i]);
			if (shifted) {
				printf(RESET_TEXT_FORMAT);
				shifted = 0;
			}
		}
		
	}
	
}


// Updates the string that will be passed to the draw_row function
void update_str(char str[STR_SIZE], char c) {
	int i;
	
	// The string will consist entirely of '#' if the char received is '#'
	if (c == '#') {
		for (i = 0; i < STR_SIZE; i++){
			str[i] = '#';
		}
	}
	
	else {
		// The char 'e' will be the divider. If the draw_board function needs to print the column index, then 'e' will simply be a space
		// Otherwise, it'll be a '#' to properly separate the slots
		char e; 
		if (c >= 'A' && c < 'A' + MAX_MATRIX) e = ' ';
		else e = '#';
		for (i = 0; i < STR_SIZE; i++) {
			// Assign 'e' to the beginning and end of the string
			if (i == 0 || i == STR_SIZE - 1) str[i] = e; 
			// Assign the received char to the middle of the string
			else if (STR_SIZE / 2 == i) str[i] = c;
			else str[i] = ' '; 
		}
	}
}


// Prints current and target sum for levels with summation
void draw_summation(int current_sum, int target_sum) {
	int size = 1, start, end, i;
	
	// Determine the length of the formatted summation (current_sum/target_sum)
	char output[STR_SIZE];
	if (current_sum >= 10) size += 2;
	else size += 1;
	if (target_sum >= 10) size += 2;
	else size += 1;
	
	// Format the summation as a string and store it in 'output'
	sprintf(output, "%d/%d", current_sum, target_sum);
	
	// Calculate the padding needed to center the summation
	start = (STR_SIZE - size) / 2; // Spaces before the summation
	end = STR_SIZE - size - start; // Spaces after the summation
	
	// Print spaces before the summation to center it
	for (i = 0; i < start; i++) {
		printf("%3c", ' ');
	}
	
	// Print the summation character by character
	for (i = 0; i < size; i++) {
		printf("%3c", output[i]);
	}
	
	// Print spaces after the summation
	for (i = 0; i < end; i++) {
		printf("%3c", ' ');
	}
}


void draw_board(BOARD board, int transition) {
	int row = 0, col = 0, state, sum, sum_index = 0, i;
	char str[STR_SIZE], color[COLOR_SIZE];
	printf("\n%3c", ' ');
	strcpy(color, "0");
	
	// Print column headers (A, B, C, etc.)
	for (col = 0; col < board.matrix; col++){
		update_str(str, 'A' + col);
		draw_row(str, '0', color, board.state);
	}
	
	// Draw the top border of the board
	printf("\n%3c", ' ');
	update_str(str, '#');
	for (col = 0; col < board.matrix; col++){
		draw_row(str, '0', color, board.state);
	}
	
	// Iterate over rows of the board to display its contents
	for (row = 0; row < board.matrix; row++){
		sum = 0; // Reset sum for the row
	
		// Change the text color if the game state is not PLAYING
		if (board.state != PLAYING) printf(colors[board.state]);
		
		// Print the row label (1, 2, 3, etc.)
		printf("\n%3c", '1' + row);
		
		// Iterate over columns in the current row
		for (col = 0; col < board.matrix; col++){
			update_str(str, board.placed_piece[row][col]); // Update str with the current piece
			
			// Determine the color of the current piece if the game is in PLAYING state
			if (board.state == PLAYING) {
				strcpy(color, colors[sprite_color(board.target, board.placed_piece[row][col], board.special, row, col, board.special_count)]); 
			}
			
			// Update the sum based on the value of the placed piece
			if (board.placed_piece[row][col] == 'X') sum += 2;
			else if (board.placed_piece[row][col] == 'O') sum += 1;
			else if (board.placed_piece[row][col] >= '0' && board.placed_piece[row][col] <= '9') sum += board.placed_piece[row][col] - '0';
			
			draw_row(str, board.placed_piece[row][col], color, board.state);
		}
		
		// Draw the summation for the current row if applicable
		if (board.conditions.summation) {
			draw_summation(sum, board.conditions.sum[sum_index]);
			sum_index++;	
		} 
		
		// Draw the row border	
		printf("\n%3c", ' ');
		update_str(str, '#');
		strcpy(color, "0"); // Reset color for the border
		
		for (col = 0; col < board.matrix; col++){
			state = board.state;
			
			// Check for special slots and adjust state if applicable
			if (board.state == PLAYING && board.conditions.has_colored_slot) {
				for (i = 0; i < board.special_count; i++){
					if (board.special[i].row == row && board.special[i].column == col) {
						if (board.special[i].type == GREEN_SLOT) state = GREEN;
						else if	(board.special[i].type == RED_SLOT) state = RED;
					}
				}
			}
			draw_row(str, '0', color, state); // Draw the border slot
		}
	}
	
	// Handle summations for columns, if applicable
	if (board.conditions.summation) {
		printf("\n");
		printf("\n%3c", ' ');
		for (col = 0; col < board.matrix; col++) {
			sum = 0; // Reset sum for the column
			
			// Calculate column summation
			for (row = 0; row < board.matrix; row++) {
				if (board.placed_piece[row][col] == 'X') sum += 2;
				else if (board.placed_piece[row][col] == 'O') sum += 1;
				else if (board.placed_piece[row][col] >= '0' && board.placed_piece[row][col] <= '9') sum += board.placed_piece[row][col] - '0';
			}
			draw_summation(sum, board.conditions.sum[sum_index]); // Draw column summation
			sum_index++;	
		}
	} 
	
	// Print the transition board when needed
	if (transition) printf("\n\n     |\n     |\n     V\n");
	else {
		printf("\n\nlvl %d\n", board.lvl);
		
		// If the game is in PLAYING state, display additional gameplay instructions
		if (board.state == PLAYING) {
			// Inform that the level has the swap condition, when applicable
			if (board.conditions.swap > 2) {
				printf(ANSI_RED);
				printf("!->");
				printf(ANSI_GREEN);
				printf("<-!");
				printf(RESET_TEXT_FORMAT);
			}
			
			// Display the next piece
			printf("\nnext: ", board.next);
			if (board.next == board.target) printf(ANSI_GREEN);
			else if (board.next == board.wrong) printf(ANSI_RED);
			printf("%c\n\n", board.next);
			printf(RESET_TEXT_FORMAT);
			
			// Print controls for the user
			printf("play: column+row (ex: a1)\nlevels map: m\nexit: 0\n\n");
		} 	
	}
}


// Clears a slot and draws the transition if needed
void clear_slot(BOARD *board, int row, int col, int transition) {
	if (transition) draw_board(*board, 1);
	board->placed_piece[row][col] = ' ';
}


// Moves a piece from a slot to another and clears the first slot
void move_piece(BOARD *board, int row, int col, int previous_row, int previous_col, int transition) {
	int i;
	
	// Optionally redraw the board to show the transition
	if (transition) draw_board(*board, 1);
	
	// Move the piece from the previous position to the new position
	board->placed_piece[row][col] = board->placed_piece[previous_row][previous_col];
	
	// Update the position of any special slot associated with the piece (except green and red slots that never change their position)
	for (i = 0; i < board->special_count; i++) {
		if (board->special[i].row == previous_row && board->special[i].column == previous_col && board->special[i].type != GREEN_SLOT && board->special[i].type != RED_SLOT)  {
			board->special[i].row = row;
			board->special[i].column = col;
		}
	}
	// Clear the previous slot
	clear_slot(board, previous_row, previous_col, 0);
}


// This function is associated with the pursuer movement: it moves and pushes all the pieces in the same direction
void update_pos(BOARD *board, int row, int col, int previous_row, int previous_col, int transition) {
	// Check if the target slot is occupied and within the matrix
	if (board->placed_piece[row][col] != ' ' && in_matrix(row, col, board->matrix)) {
		int dif_row = previous_row - row, dif_col = previous_col - col;
		transition = 0; // Suppress transition redraws for recursion
		// Recursively move the occupying piece further in the same direction
		update_pos(board, row - dif_row, col - dif_col, row, col, 1);
	}
	
	// If the target position is valid, move the piece; otherwise, draw the transition
	if (in_matrix(row, col, board->matrix)) {
		move_piece(board, row, col, previous_row, previous_col, transition);
	} 
	else draw_board(*board, 1);
}


// Updates the pursuer's position towards the target row or column
void update_pursuer(int p[2], int p_row, int p_col, int target_row, int target_col) {
	// If it's not in the same row as the target, it'll move up or down; otherwise, it'll move left or right
	// That means it prioritizes going to the same row before going to the same column (it can only move one slot at a time, and no diagonal movement)
	if (p_row != target_row) {
		if (p_row > target_row) p[ROW] = p_row - 1; // Move up
		else p[ROW] = p_row + 1; // Move down
		p[COLUMN] = p_col; // Maintain column
	}
	else {
		if (p_col > target_col) p[COLUMN] = p_col - 1; // Move left
		else p[COLUMN] = p_col + 1; // Move right
		p[ROW] = p_row; // Maintain row
	}		
}


// Determines the pursuer target and moves it towards that target; rules to determine the target: 
// It must be a target sprite 
// The pursuer targets the closest target sprite 
// If there's more than one target sprite within the same radius, it targets the one that comes first in the board's matrix
BOARD move_pursuer(BOARD board, int p_row, int p_col) {
	int max_dist = (board.matrix - 1) * 2, dist, dist_row, radius = 1, row, col, next[] = {-1, -1};
	
	// Incrementally search for the target piece within increasing radius
	while (radius <= max_dist) {
		for (row = 0; row < board.matrix; row++){
			dist_row = abs(p_row - row); // Row distance from the pursuer
			for (col = 0; col < board.matrix; col++) {
				dist = dist_row + abs(p_col - col); // Total Manhattan distance
				if (dist == radius) {
					// Check if the piece at this position is the target sprite
					if (board.placed_piece[row][col] == board.target) {
						update_pursuer(next, p_row, p_col, row, col); // Update pursuer's position
						break; 
					}
				}
			}
			if (next[COLUMN] != -1) break; // Exit early if target is found
		}
		if (next[COLUMN] != -1) break; // Exit early if target is found
		radius++; // Expand the search radius
	}
	
	// If a valid next position is found, update the board with the pursuer's move
	if (next[COLUMN] != -1) update_pos(&board, next[ROW], next[COLUMN], p_row, p_col, 1);
	
	return board;	
}


// Swaps the piece at the given position to its opposite sprite and draws the transition when needed
void swap_piece(BOARD *board, int row, int col, int transition) {
	if (transition) draw_board(*board, 1);
	board->placed_piece[row][col] = opposite_sprite(board->placed_piece[row][col]);
}


// Applies the provided function to all the slots adjacent to the given position, and draws the transition only for the first function call
void special_in_common(BOARD *board, int row, int col, void (*function)(BOARD *board, int row, int col, int tran)) {
	int i, coord[] = {row + 1, col, row - 1, col, row, col + 1, row, col - 1}, transition = 1;
	
	// Loop through all adjacent slots
	for (i = 0; i < 8; i += 2) {
		// Check if the slot is within the matrix, has no special elements, and is not empty
		if (in_matrix(coord[i], coord[i + 1], board->matrix) && no_special(board->special, coord[i], coord[i + 1], board->special_count) && board->placed_piece[coord[i]][coord[i + 1]] != ' ') {
			function(board, coord[i], coord[i + 1], transition);
			transition = 0;	// Disable transition redraws for subsequent function calls
		}
	}
}


// Applies the "swap_piece" function to all adjacent pieces
BOARD transmute(BOARD board, int row, int col) {
	special_in_common(&board, row, col, swap_piece);
	return board;
}


// Updates bomb timer and activates bomb power when ready to explode
BOARD activate_bomb(BOARD board, int row, int col) {
	int i = 0;
	
	// Find the bomb special object at the given position
	while (i < board.special_count) {
		if (board.special[i].type == BOMB) {
			if (board.special[i].row == row && board.special[i].column == col) break;
		}
		i++;
	}
	
	// Draw a transition to indicate the activation of the bomb
	draw_board(board, 1);
	
	// Decrement the bomb timer
	board.special[i].timer -= 1;
	
	// Update the piece at the bomb's position to reflect the timer value
	board.placed_piece[row][col] = board.special[i].timer + '0';
	
	// If the timer reaches 0, activate the bomb's effect
	if (board.special[i].timer == 0) {
		// Clear all adjacent slots using the "clear_slot" function
		special_in_common(&board, row, col, clear_slot);
		
		// Reset the bomb timer to its original value
		board.special[i].timer = board.special[i].original_timer;
		
		// Update the piece to reflect the reset timer
		board.placed_piece[row][col] = board.special[i].timer + '0';
	}
	
	return board;
}

// Swaps all the valid pieces to their opposite sprite and draws the transition only before the first swap
BOARD swap_all(BOARD board) {
	int row, col, transition = 1;
	
	// Iterate through the entire matrix
	for (row = 0; row < board.matrix; row++) {
		for (col = 0; col < board.matrix; col++) {
			// Check if the piece is 'O' or 'X' and has no special elements
			if (board.placed_piece[row][col] == 'O' || board.placed_piece[row][col] == 'X') {
				if (no_special(board.special, row, col, board.special_count)) {
					// Swap the piece to its opposite sprite
					swap_piece(&board, row, col, transition);
					// Disable transition redraws for subsequent swaps
					transition = 0;	
				} 
			}		
		}
	}
	
	return board;
}


// Moves PMan to the next slot according to its current direction
BOARD move_pman(BOARD board, int row, int col) {
	int movement, reset_col, i = 0;
	
	// Locate the PMAN special object in the board's special array
	while (i < board.special_count) {
		if (board.special[i].type == PMAN) {
			if (board.special[i].row == row && board.special[i].column == col) break;	
		}
		i++;
	}
	
	// Determine the direction of movement based on PMAN's current direction
	if (board.special[i].direction == RIGHT) {
		movement = 1; // Move to the right and down
		reset_col = 0; // Wrap around to the first column
	}
	else {
		movement = - 1; // Move to the left and up
		reset_col = board.matrix - 1; // Wrap around to the last column
	}
	
	// Try moving PMAN to the next column in the current direction
	if (in_matrix(row, col + movement, board.matrix) && no_special(board.special, row, col + movement, board.special_count)) {
		move_piece(&board, row, col + movement, row, col, 1);
	}
	// If the next column is blocked, try wrapping to the next row in the opposite column
	else if (in_matrix(row + movement, reset_col, board.matrix) && no_special(board.special, row + movement, reset_col, board.special_count)) {
		move_piece(&board, row + movement, reset_col, row, col, 1);
	}
	// If both options fail, reverse PMAN's direction and try again
	else {
		if(board.special[i].direction == RIGHT) board.special[i].direction = LEFT;
		else board.special[i].direction = RIGHT;
		board = move_pman(board, row, col); // Recursively retry the move with the reversed direction
	}	
	
	return board;
}


// Adjusts the provided coordinate to bring it one step closer to "target"
int update_magnetized(int target, int coord) {
	if (target > coord) return coord + 1;
	if (target < coord) return coord - 1;
	return coord;
}


// Moves the magnetized piece to a valid position, if applicable
void magnetize(BOARD *board, int magnet_row, int magnet_col, int current_row, int current_col) {
	// Determine the target position by "magnetizing" the current position
	int target_row = update_magnetized(magnet_row, current_row), target_col = update_magnetized(magnet_col, current_col); 
	
	// If the target position is empty, move the piece there
	if (board->placed_piece[target_row][target_col] == ' ') move_piece(board, target_row, target_col, current_row, current_col, 1);
	
	// If the target is occupied, try alternative paths
	else {
		int dist_row = abs(magnet_row - current_row), dist_col = abs(magnet_col - current_col), moved = 0;
		
		// Prioritize vertical movement if the vertical distance is greater
		if (dist_row > dist_col) {
			if (board->placed_piece[target_row][current_col] == ' ') {
				move_piece(board, target_row, current_col, current_row, current_col, 1);
				moved = 1;
			}
		}
		
		// If vertical distance isn't greater or vertical movement wasn't possible, try horizontal movement
		if (!moved) {
			if (board->placed_piece[current_row][target_col] == ' ') {
				move_piece(board, current_row, target_col, current_row, current_col, 1);
				moved = 1;
			}
			
			// Finally, fallback to vertical movement if horizontal movement also failed (the piece won't move at all if this also fails)
			else if (!moved && board->placed_piece[target_row][current_col] == ' ') move_piece(board, target_row, current_col, current_row, current_col, 1);	
		}
	}
}


// Activates the magnet effect with the following rules:
// Pieces closer to the magnet move first, always one slot at a time; diagonal movement is allowed
// Vertical movement is prioritized only if the vertical distance is greater, otherwise horizontal movement is prioritized
// A piece only moves if it can decrease its Manhattan distance to the magnet, and if there's available space for it to move
BOARD activate_magnet(BOARD board, int magnet_row, int magnet_col) {
	int max_dist = (board.matrix - 1) * 2, dist_row, dist, radius = 2, row, col;
	
	// Gradually increase the search radius to magnetize pieces
	while (radius <= max_dist) {
		for (row = 0; row < board.matrix; row++){
			dist_row = abs(magnet_row - row); // Vertical distance to magnet
			for (col = 0; col < board.matrix; col++) {
				dist = dist_row + abs(magnet_col - col); // Total Manhattan distance
				
				// Check if the piece is at the current radius
				if (dist == radius) {
					// If the position is occupied, attempt to magnetize the piece
					if (board.placed_piece[row][col] != ' ') {
						magnetize(&board, magnet_row, magnet_col, row, col);
					}
				}
			}
		}
		radius++; // Expand the search radius
	}
	
	return board;
}


// Returns the validity of the move (-1 = exit, 0 = invalid, 1 = valid, 2 = levels map, 3 = occupied position)
int valid_move(char move[INPUT_SIZE], BOARD board) {
	// Check if the input is an exit command
	if (move[0] == '0') return -1;
	
	// Check if the input is a command to see the levels map
	if (move[0] == 'M') return 2;
	
	// Validate the column input (must be within 'A' to the last column letter)
	if (move[COLUMN] < 'A' || move[COLUMN] > 'A' + board.matrix - 1) return 0;
	
	// Validate the row input (must be within '1' to the last row number)
	if (move[ROW] < '1' || move[ROW] > '1' + board.matrix - 1) return 0;
	
	// Convert the column and row input from characters to indices
	move[COLUMN] -= 'A'; 
	move[ROW] -= '1'; 
	
	// Check if the target position is occupied
	if (board.placed_piece[move[ROW]][move[COLUMN]] != ' ') return 3;
	
	// The move is valid
	return 1; 
}


int play(char move[INPUT_SIZE], BOARD *board) {
	int valid = valid_move(move, *board);
	
	// Exit the game if the move indicates an exit command
	if (valid < 0) exit(0);
	
	// Return 2 for displaying the levels map
	if (valid == 2) return 2; 
	
	// Clear input buffer to ensure clean user interaction
	scanf("%*[^\n]");
	scanf("%*c");
	
	// Handle invalid moves
	if (valid == 0) {
		printf("invalid command\n\n"); // Invalid input format
	}
	else if (valid == 3) {
		printf("invalid move: occupied position\n\n"); // Position already occupied
	}
	else {
		// Make the move on the board
		board -> placed_piece[move[ROW]][move[COLUMN]] = board -> next;
		
		// Handle the swapping logic for the next piece as follows:
		// 0: no swapping, so the pieces always alternate and the swap condition stays the same
		// 1 and 3: there's a transmuter or swap condition and the piece is the first of its kind, so the next one is the same piece and the swap condition increases
		// 2 and 4: there's a transmuter or swap condition and the piece is the second of its kind, so the next one is the opposite piece and the swap condition decreases
		if (board->conditions.swap % 2 == 0) {
			board -> next = opposite_sprite(board -> next);
			if (board->conditions.swap) board->conditions.swap -= 1;
		}
		else board->conditions.swap += 1;
		
		return 1;	
	} 
	
	// Prompt the user to press ENTER to continue when the input is invalid
	printf("press ENTER to continue");
	scanf("%*c");
	printf("\n");
	return 0;	
} 


// Clears all the slots sprites, sets all special elements to NONE and sets the board state to PLAYING
void reset_board(BOARD *board){
	int row, col, i;
	for (row = 0; row < board->matrix; row++){
		for (col = 0; col < board->matrix; col++){
			board->placed_piece[row][col] = ' ';
			for (i = 0; i < MAX_SP; i++) {
				board->special[i].type = NONE;
			}
		}
	}
	board->state = PLAYING;
}


// Counts empty slots in the board matrix
int update_free_slots(BOARD board) {
	int row, col, free_slots = 0;
	for (row = 0; row < board.matrix; row++) {
		for (col = 0; col < board.matrix; col++) {
			if (board.placed_piece[row][col] == ' ') free_slots += 1;
		}
	}
	return free_slots;
}


// Loads the specified level from the "lvls.txt" file
void load_lvl(const char *file, int lvl_number, BOARD *board) {
	int i, j, sprite;
    FILE *fp = fopen(file, "r");
    
    // Check if the file opened successfully
    if (!fp) {
        perror("error opening the file");
        exit(1);
    }
    
    int count = 1;
    // Read through the file to find the desired level
    while (!feof(fp)) {
	    fscanf(fp, "%d %d %d", &board->matrix, &board->special_count, &board->conditions.summation);
	    
	    // Check if the current level matches the requested level number
        if (count == lvl_number) {
        	// Read level configurations that exist to all levels
            fscanf(fp, "%d %d %d %d %d %d", &board->free_slots, &board->next, &board->target, &board->wrong, &board->conditions.has_colored_slot, &board->conditions.swap);
            
            // Reset the board before loading level configurations that only specific levels have
            reset_board(board);
            
            // Read special elements and place them on the board
			for (i = 0; i < board->special_count; i++) {
            	fscanf(fp, "%d %d %d %d %d %d", &board->special[i].type, &board->special[i].row, &board->special[i].column, &board->special[i].timer, &board->special[i].direction, &sprite);
            	board->special[i].original_timer = board->special[i].timer;
            	board->placed_piece[board->special[i].row][board->special[i].column] = sprite;
			}
			
			// If the level has summation conditions, load them
			if (board->conditions.summation) {
				for (i = 0; i < board->matrix; i++) {
					fscanf(fp, "%d", &board->conditions.sum[i]);
				}
				for (j = 0; j < board->matrix; j++) {
					fscanf(fp, "%d", &board->conditions.sum[i]);
					i++;
				}
			}
			
            fclose(fp);
            board->lvl = lvl_number;
            return;
        }
        
        // Skip unused data for levels that don't match the desired level
        fscanf(fp, "%*d %*d %*d %*d %*d %*d");
        for (i = 0; i < board->special_count; i++) {
        	fscanf(fp, "%*d %*d %*d %*d %*d %*d");
		}
		if (board->conditions.summation) {
			for (i = 0; i < board->matrix; i++) {
				fscanf(fp, "%*d");
			}
			for (j = 0; j < board->matrix; j++) {
				fscanf(fp, "%*d");
			}
		}
        count++;
    }
    
    fclose(fp);
    printf("error loading level %d\n", lvl_number);
    exit(1);
}


// Array with special functions
BOARD (*TTO_FUNCTIONS_H_functions_sp[])(BOARD, int, int) = {move_pursuer, move_pman, activate_bomb, transmute, activate_magnet};


int game_loop (BOARD *board, int activate, int last_lvl) {
	int i, activated = 0, lvl_ended = 0, send_back = 0;
	
	// Check if we are not in the secret last level
	if (last_lvl <= MAX_LVL) {
		// If there's a winner or no free slots left, the game is lost (because the main function already tested if the level was completed)
		if (winner(*board) || !board->free_slots){ // "winner()" from TTO_conditions.h
			board->state = LOST; // Update game state to LOST
			draw_board(*board, 0); // Draw the final board state (will be printed in red)
			printf("\n");
			load_lvl("lvls.txt", board->lvl, board); // Reload the current level
			return 0;	
		}	
	}
	
	// Activation phase for special elements or board conditions	
	if (activate) {
		for (i = 0; i < board->special_count; i++) {
			// Check if the special element type is below GREEN_SLOT (valid)
			if (board->special[i].type < GREEN_SLOT) {
				activated = 1;
				// Execute the corresponding function for the special element
				*board = TTO_FUNCTIONS_H_functions_sp[board->special[i].type](*board, board->special[i].row, board->special[i].column);	
			}
		}
		
		// Handle swap condition if applicable
		if (board->conditions.swap > 2) {
			*board = swap_all(*board);
			activated = 1;	
		} 
		
		// Check for a winner if any action was activated
		if (activated) {
			if (winner(*board)) lvl_ended = 1; // Mark the level as ended if the board has a winner
		}			
	}
	
	// If the level hasn't ended, prompt for the player's move
	if (!lvl_ended){
		char move[INPUT_SIZE];
		draw_board(*board, 0); // Display the current state of the board
		fgets(move, INPUT_SIZE, stdin); // Get the player's input
		strupr(move); // Convert the input to uppercase for consistency
		send_back = play(move, board); // Process the move
		board->free_slots = update_free_slots(*board); // Update the number of free slots
	}
	
	return send_back; // Return the result of this game loop iteration
}


// Initializes the levels' states based on the current level being played
void initialize_lvls_state(int lvls[MAX_LVL], int current_lvl) {
	int i;
	for (i = 0; i < current_lvl - 1; i++) {
		lvls[i] = LVL_COMPLETED;
	}
	if (current_lvl <= MAX_LVL) {
		lvls[i] = LVL_PLAYING;
		i++;
		for (; i < MAX_LVL; i++) {
			lvls[i] = LVL_NOT_PLAYED;
		}
	}	
}


// Updates the state of levels and their associated quadrants
void update_lvls(int lvls[MAX_LVL], int quadrants[N_QUADRANTS], int lvl, int state) {
	int quadrant = 1, i;
	
	while (N_QUADRANTS * quadrant < lvl) {
		quadrant++; // Determine which quadrant the level belongs to
	}
	
	quadrants[quadrant - 1] = state; // Update the quadrant's state
	
	quadrant *= N_QUADRANTS; // Calculate the range of levels in the quadrant
	for (i = quadrant - N_QUADRANTS; i < quadrant; i++) {
		lvls[i] = state; // Update all levels within the quadrant
	} 
}


// Loads the save file or creates a new one if it doesn’t exist
int load_save() {
    const char *filename = "save.bin";
    FILE *file;
	int lvl, i;
    file = fopen(filename, "rb");
    if (file) {
        fread(&lvl, sizeof(int), 1, file);
        fclose(file);
    } else {
        file = fopen(filename, "wb");
        if (file) {
        	lvl = 1;
            fwrite(&lvl, sizeof(int), 1, file);
            fclose(file);
        } else {
            perror("\nerror creating save file");
            exit(1);
        }
    }
    return lvl;
}


// Saves the current level to a binary file
void save(int lvl) {
	const char *filename = "save.bin";
    FILE *file;
    file = fopen(filename, "wb");
    if (file) {
        fwrite(&lvl, sizeof(int), 1, file);
        fclose(file);
    } else {
        perror("\nerror creating save file");
        exit(1);
    }
}


// Displays a menu for replaying completed levels or returning to the last level
int map_menu(int current_lvl) {
	int choice, confirm;
	do {
		printf("\npick a completed level to replay or 0 to go back: ");
		scanf("%d", &choice);	
	} while (choice < 0 || choice > current_lvl || choice > MAX_LVL);
	scanf("%*[^\n]");
	scanf("%*c");
	if (choice && current_lvl <= MAX_LVL) {
		printf("are you sure?\nprogress in current level will be lost\n");
		do {
			printf("0: CANCEL\n1: confirm\n");
			scanf("%d", &confirm);
		} while (confirm < 0 || confirm > 1);
		scanf("%*[^\n]");
		scanf("%*c");
		if (!confirm) return 0;
	}
	return choice;
}


// Displays the map of levels and their states
int map(int lvl, int lvls[MAX_LVL], int call_menu) {
	int i, output = 1, control = 1, choice;
	printf("\n");
	
	for (i = 1; i <= MAX_LVL; i++) {
		// Color-code the levels based on their state
		if (lvls[output - 1] == LVL_COMPLETED) printf(ANSI_GREEN);	
		else if (lvls[output - 1] == LVL_PLAYING) printf(ANSI_YELLOW);
		else if (lvls[output - 1] == LVL_RED) printf(ANSI_RED);
		else printf(RESET_TEXT_FORMAT);
		
		// Print level numbers with formatting
		if (output < 10) printf ("%1c%1c%1c", '0', output + '0', ' ');
		else printf("%1c%1c%1c", output / 10 + '0', output % 10 + '0', ' ');
		output += 1;
		
		// Handle layout and formatting
		if (i % BLOCK_DIVISOR == 0) {
			printf("\n\n");
			control = 1;
		} 	
		else if (i % ROW_DIVISOR == 0) {
			printf("\n");
			output = i + 1 - COLUMN_DIVISOR * (COLUMN_DIVISOR - 1) * control; 
			control += 1;	
		}
		else if (i % COLUMN_DIVISOR == 0) {
			printf("%1c", ' '); 
			output += COLUMN_DIVISOR * (COLUMN_DIVISOR - 1);
		} 
	}
	
	// Print special case for the secret final level
	if (lvl >= MAX_LVL) {
		printf(RESET_TEXT_FORMAT);
		if (lvl > MAX_LVL && call_menu) {
			printf("\nlvl %d?\n", MAX_LVL + 1);
			printf(ANSI_GREEN);
			printf("red\n");
			printf(RESET_TEXT_FORMAT);
		}	
	}
	
	// Optional level selection menu
	if (call_menu) choice = map_menu(lvl);
	
	return choice;
}


// Displays the main menu and handles user choices
int menu(int lvl) {
	int choice, max_choice = 2;
	char msg[40] = "1: continue\n2: new game\n0: exit";
	
	if (lvl == 1) {
		strcpy(msg, "1: new game\n0: exit");
		max_choice = 1; // No "continue" option if it's the first level
	}
	
	do {
		printf("%s\n\n", msg);
		scanf("%d", &choice);
		scanf("%*[^\n]");
		scanf("%*c");	
		
		if (lvl > 1 && choice == 2) {
			// Prompt confirmation for starting a new game
			printf("\nare you sure you want to start a new game?\nyour save file will be overwitten\n\n0: ERASE saved game and start a new one\n1: cancel\n");
			scanf("%d", &choice);	
			scanf("%*[^\n]");
			scanf("%*c");
			if (choice == 0) return 2;
			choice = -1;
		}
		
	} while (choice < 0 || choice > max_choice);
	
	return choice;
}


#endif // TTO_FUNCTIONS_H
