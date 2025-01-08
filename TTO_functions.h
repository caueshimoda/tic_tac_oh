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


// Returns the oposite piece from the received one
char opposite_sprite(char c){
	if (c == 'X') return 'O';
	return 'X';
}


int sprite_color(char target, char current, SPECIAL sp[MAX_SP], int row, int col, int sp_qty) {
	int i, color = 0;
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
					break;
				} 
			}
		}
	}
	if (color) return color;
	if (target != ' ') {
		if (current == target) return TARGET_COLOR;
		if (current == opposite_sprite(target)) return WRONG_COLOR;
	}
	return STD_COLOR; 
}


int in_matrix(int row, int col, int matrix) {
	if (row < 0 || row >= matrix) return 0;
	if (col < 0 || col >= matrix) return 0;
	return 1;
}


int no_special(SPECIAL sp[MAX_SP], int row, int col, int qty) {
	int i;
	for (i = 0; i < qty; i++) {
		if (sp[i].type < GREEN_SLOT) {
			if (sp[i].row == row && sp[i].column == col) return 0;	
		}
	}
	return 1;
}


/* Draws part of the current row from the given string.
If there is a valid color, the character provided will be painted with it.
*/
void draw_row(char str[STR_SIZE], char c, char color[COLOR_SIZE], int state) {
	int i;
	if (state != PLAYING){
		if (state == WON) printf(ANSI_GREEN);
		else printf(ANSI_RED);
		for (i = 0; i < STR_SIZE; i++){
			printf("%3c", str[i]);
		}
		printf(RESET_TEXT_FORMAT);
	}
	else {
		int shifted = 0, comp = strcmp(color, "0");
		for (i = 0; i < STR_SIZE; i++){
			if (str[i] == c && comp != 0) {
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


void update_str(char str[STR_SIZE], char c) {
	int i;
	if (c == '#') {
		for (i = 0; i < STR_SIZE; i++){
			str[i] = '#';
		}
	}
	else {
		char e;
		if (c >= 'A' && c < 'A' + MAX_MATRIX) e = ' ';
		else e = '#';
		for (i = 0; i < STR_SIZE; i++) {
			if (i == 0 || i == STR_SIZE - 1) str[i] = e;
			else if (STR_SIZE / 2 == i) str[i] = c;
			else str[i] = ' '; 
		}
	}
	
	
}


void draw_summation(int current_sum, int target_sum) {
	int size = 1, start, end, i;
	char output[STR_SIZE];
	if (current_sum >= 10) size += 2;
	else size += 1;
	if (target_sum >= 10) size += 2;
	else size += 1;
	sprintf(output, "%d/%d", current_sum, target_sum);
	start = (STR_SIZE - size) / 2;
	end = STR_SIZE - size - start;
	for (i = 0; i < start; i++) {
		printf("%3c", ' ');
	}
	for (i = 0; i < size; i++) {
		printf("%3c", output[i]);
	}
	for (i = 0; i < end; i++) {
		printf("%3c", ' ');
	}
}


void draw_board(BOARD board, int transition) {
	int row = 0, col = 0, state, sum, sum_index = 0, i;
	char str[STR_SIZE], color[COLOR_SIZE];
	printf("\n%3c", ' ');
	strcpy(color, "0");
	for (col = 0; col < board.matrix; col++){
		update_str(str, 'A' + col);
		draw_row(str, '0', color, board.state);
	}
	printf("\n%3c", ' ');
	update_str(str, '#');
	for (col = 0; col < board.matrix; col++){
		draw_row(str, '0', color, board.state);
	}
	for (row = 0; row < board.matrix; row++){
		sum = 0;
		if (board.state != PLAYING) printf(colors[board.state]);
		printf("\n%3c", '1' + row);
		for (col = 0; col < board.matrix; col++){
			update_str(str, board.placed_piece[row][col]);
			if (board.state == PLAYING) {
				strcpy(color, colors[sprite_color(board.target, board.placed_piece[row][col], board.special, row, col, board.special_count)]);
			}
			if (board.placed_piece[row][col] == 'X') sum += 2;
			else if (board.placed_piece[row][col] == 'O') sum += 1;
			else if (board.placed_piece[row][col] >= '0' && board.placed_piece[row][col] <= '9') sum += board.placed_piece[row][col] - '0';
			draw_row(str, board.placed_piece[row][col], color, board.state);
		}
		if (board.conditions.summation) {
			draw_summation(sum, board.conditions.sum[sum_index]);
			sum_index++;	
		} 	
		printf("\n%3c", ' ');
		update_str(str, '#');
		strcpy(color, "0");
		for (col = 0; col < board.matrix; col++){
			state = board.state;
			if (board.state == PLAYING && board.conditions.has_colored_slot) {
				for (i = 0; i < board.special_count; i++){
					if (board.special[i].row == row && board.special[i].column == col) {
						if (board.special[i].type == GREEN_SLOT) state = GREEN;
						else if	(board.special[i].type == RED_SLOT) state = RED;
					}
				}
			}
			draw_row(str, '0', color, state);
		}
	}
	if (board.conditions.summation) {
		printf("\n");
		printf("\n%3c", ' ');
		for (col = 0; col < board.matrix; col++) {
			sum = 0;
			for (row = 0; row < board.matrix; row++) {
				if (board.placed_piece[row][col] == 'X') sum += 2;
				else if (board.placed_piece[row][col] == 'O') sum += 1;
				else if (board.placed_piece[row][col] >= '0' && board.placed_piece[row][col] <= '9') sum += board.placed_piece[row][col] - '0';
			}
			draw_summation(sum, board.conditions.sum[sum_index]);
			sum_index++;	
		}
	} 
	if (transition) printf("\n\n     |\n     |\n     V\n");
	else {
		printf("\n\nlvl %d\n", board.lvl);
		if (board.state == PLAYING) {
			if (board.conditions.swap > 2) {
				printf(ANSI_RED);
				printf("!->");
				printf(ANSI_GREEN);
				printf("<-!");
				printf(RESET_TEXT_FORMAT);
			}
			printf("\nnext: ", board.next);
			if (board.next == board.target) printf(ANSI_GREEN);
			else if (board.next == board.wrong) printf(ANSI_RED);
			printf("%c\n\n", board.next);
			printf(RESET_TEXT_FORMAT);
			printf("play: column+row (ex: a1)\nlevel map: m\nexit: 0\n\n");
		} 	
	}
}


void clean_slot(BOARD *board, int row, int col, int transition) {
	if (transition) draw_board(*board, 1);
	board->placed_piece[row][col] = ' ';
}


void move_piece(BOARD *board, int row, int col, int previous_row, int previous_col, int transition) {
	int i;
	if (transition) draw_board(*board, 1);
	board->placed_piece[row][col] = board->placed_piece[previous_row][previous_col];
	for (i = 0; i < board->special_count; i++) {
		if (board->special[i].row == previous_row && board->special[i].column == previous_col && board->special[i].type != GREEN_SLOT && board->special[i].type != RED_SLOT)  {
			board->special[i].row = row;
			board->special[i].column = col;
		}
	}
	clean_slot(board, previous_row, previous_col, 0);
}


void update_pos(BOARD *board, int row, int col, int previous_row, int previous_col, int transition) {
	if (board->placed_piece[row][col] != ' ' && in_matrix(row, col, board->matrix)) {
		int dif_row = previous_row - row, dif_col = previous_col - col;
		transition = 0;
		update_pos(board, row - dif_row, col - dif_col, row, col, 1);
	}
	if (in_matrix(row, col, board->matrix)) {
		move_piece(board, row, col, previous_row, previous_col, transition);
	} 
	else draw_board(*board, 1);
}


void update_pursuer(int p[2], int p_row, int p_col, int target_row, int target_col) {
	if (p_row != target_row) {
		if (p_row > target_row) p[ROW] = p_row - 1;
		else p[ROW] = p_row + 1;
		p[COLUMN] = p_col;
	}
	else {
		if (p_col > target_col) p[COLUMN] = p_col - 1;
		else p[COLUMN] = p_col + 1;
		p[ROW] = p_row;
	}		
}



BOARD move_pursuer(BOARD board, int p_row, int p_col) {
	int max_dist = (board.matrix - 1) * 2, dist, dist_row, radius = 1, row, col, next[] = {-1, -1};
	while (radius <= max_dist) {
		for (row = 0; row < board.matrix; row++){
			dist_row = abs(p_row - row);
			for (col = 0; col < board.matrix; col++) {
				dist = dist_row + abs(p_col - col);
				if (dist == radius) {
					if (board.placed_piece[row][col] == board.target) {
						update_pursuer(next, p_row, p_col, row, col);
						break;
					}
				}
			}
			if (next[COLUMN] != -1) break;
		}
		if (next[COLUMN] != -1) break; 
		radius++;	
	}
	if (next[COLUMN] != -1) update_pos(&board, next[ROW], next[COLUMN], p_row, p_col, 1);
	return board;	
}


void swap_piece(BOARD *board, int row, int col, int transition) {
	if (transition) draw_board(*board, 1);
	board->placed_piece[row][col] = opposite_sprite(board->placed_piece[row][col]);
}


void special_in_common(BOARD *board, int row, int col, void (*function)(BOARD *board, int row, int col, int tran)) {
	int i, coord[] = {row + 1, col, row - 1, col, row, col + 1, row, col - 1}, transition = 1;
	for (i = 0; i < 8; i += 2) {
		if (in_matrix(coord[i], coord[i + 1], board->matrix) && no_special(board->special, coord[i], coord[i + 1], board->special_count) && board->placed_piece[coord[i]][coord[i + 1]] != ' ') {
			function(board, coord[i], coord[i + 1], transition);
			transition = 0;	
		}
	}
}


BOARD transmute(BOARD board, int row, int col) {
	special_in_common(&board, row, col, swap_piece);
	return board;
}


BOARD activate_bomb(BOARD board, int row, int col) {
	int i = 0;
	while (i < board.special_count) {
		if (board.special[i].type == BOMB) {
			if (board.special[i].row == row && board.special[i].column == col) break;
		}
		i++;
	}
	draw_board(board, 1);
	board.special[i].timer -= 1;
	board.placed_piece[row][col] = board.special[i].timer + '0';
	if (board.special[i].timer == 0) {
		special_in_common(&board, row, col, clean_slot);
		board.special[i].timer = board.special[i].original_timer;
		board.placed_piece[row][col] = board.special[i].timer + '0';
	}
	return board;
}


BOARD swap_all(BOARD board) {
	int row, col, transition = 1;
	for (row = 0; row < board.matrix; row++) {
		for (col = 0; col < board.matrix; col++) {
			if (board.placed_piece[row][col] == 'O' || board.placed_piece[row][col] == 'X') {
				if (no_special(board.special, row, col, board.special_count)) {
					swap_piece(&board, row, col, transition);
					transition = 0;	
				} 
			}		
		}
	}
	return board;
}


BOARD move_pman(BOARD board, int row, int col) {
	int movement, reset_col, i = 0;
	while (i < board.special_count) {
		if (board.special[i].type == PMAN) {
			if (board.special[i].row == row && board.special[i].column == col) break;	
		}
		i++;
	}
	if (board.special[i].direction == RIGHT) {
		movement = 1;
		reset_col = 0;
	}
	else {
		movement = - 1;
		reset_col = board.matrix - 1;
	}
	if (in_matrix(row, col + movement, board.matrix) && no_special(board.special, row, col + movement, board.special_count)) {
		move_piece(&board, row, col + movement, row, col, 1);
	}
	else if (in_matrix(row + movement, reset_col, board.matrix) && no_special(board.special, row + movement, reset_col, board.special_count)) {
		move_piece(&board, row + movement, reset_col, row, col, 1);
	}
	else {
		if(board.special[i].direction == RIGHT) board.special[i].direction = LEFT;
		else board.special[i].direction = RIGHT;
		board = move_pman(board, row, col);
	}	
	return board;
}


int update_magnetized(int target, int coord) {
	if (target > coord) return coord + 1;
	if (target < coord) return coord - 1;
	return coord;
}


void magnetize(BOARD *board, int magnet_row, int magnet_col, int current_row, int current_col) {
	int target_row = update_magnetized(magnet_row, current_row), target_col = update_magnetized(magnet_col, current_col);
	if (board->placed_piece[target_row][target_col] == ' ') move_piece(board, target_row, target_col, current_row, current_col, 1);
	else {
		int dist_row = abs(magnet_row - current_row), dist_col = abs(magnet_col - current_col), moved = 0;
		if (dist_row > dist_col) {
			if (board->placed_piece[target_row][current_col] == ' ') {
				move_piece(board, target_row, current_col, current_row, current_col, 1);
				moved = 1;
			}
		}
		if (!moved) {
			if (board->placed_piece[current_row][target_col] == ' ') {
				move_piece(board, current_row, target_col, current_row, current_col, 1);
				moved = 1;
			}
			else if (!moved && board->placed_piece[target_row][current_col] == ' ') move_piece(board, target_row, current_col, current_row, current_col, 1);	
		}
	}
}


BOARD activate_magnet(BOARD board, int magnet_row, int magnet_col) {
	int max_dist = (board.matrix - 1) * 2, dist_row, dist, radius = 2, row, col;
	while (radius <= max_dist) {
		for (row = 0; row < board.matrix; row++){
			dist_row = abs(magnet_row - row);
			for (col = 0; col < board.matrix; col++) {
				dist = dist_row + abs(magnet_col - col);
				if (dist == radius) {
					if (board.placed_piece[row][col] != ' ') {
						magnetize(&board, magnet_row, magnet_col, row, col);
					}
				}
			}
		}
		radius++;
	}
	return board;
}


int valid_move(char move[INPUT_SIZE], BOARD board) {
	if (move[0] == '0') return -1;
	if (move[0] == 'M') return 2;
	if (move[COLUMN] < 'A' || move[COLUMN] > 'A' + board.matrix - 1) return 0;
	if (move[ROW] < '1' || move[ROW] > '1' + board.matrix - 1) return 0;
	move[COLUMN] -= 'A'; 
	move[ROW] -= '1'; 
	if (board.placed_piece[move[ROW]][move[COLUMN]] != ' ') return 3;
	return 1; 
}


int play(char move[INPUT_SIZE], BOARD *board) {
	int valid = valid_move(move, *board);
	if (valid < 0) exit(0);
	if (valid == 2) return 2; 
	scanf("%*[^\n]");
	scanf("%*c");
	if (valid == 0) {
		printf("invalid command\n\n");
	}
	else if (valid == 3) {
		printf("invalid move: occupied position\n\n");
	}
	else {
		board -> placed_piece[move[ROW]][move[COLUMN]] = board -> next;
		if (board->conditions.swap % 2 == 0) {
			board -> next = opposite_sprite(board -> next);
			if (board->conditions.swap) board->conditions.swap -= 1;
		}
		else board->conditions.swap += 1;
		return 1;	
	} 
	printf("press ENTER to continue");
	scanf("%*c");
	printf("\n");
	return 0;	
} 


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


int update_free_slots(BOARD board) {
	int row, col, free_slots = 0;
	for (row = 0; row < board.matrix; row++) {
		for (col = 0; col < board.matrix; col++) {
			if (board.placed_piece[row][col] == ' ') free_slots += 1;
		}
	}
	return free_slots;
}


void load_lvl(const char *file, int lvl_number, BOARD *board) {
	int i, j, sprite;
    FILE *fp = fopen(file, "r");
    if (!fp) {
        perror("error opening the file");
        exit(1);
    }
    int count = 1;
    while (!feof(fp)) {
	    fscanf(fp, "%d %d %d", &board->matrix, &board->special_count, &board->conditions.summation);
        if (count == lvl_number) {
            fscanf(fp, "%d %d %d %d %d %d", &board->free_slots, &board->next, &board->target, &board->wrong, &board->conditions.has_colored_slot, &board->conditions.swap);
            reset_board(board);
			for (i = 0; i < board->special_count; i++) {
            	fscanf(fp, "%d %d %d %d %d %d", &board->special[i].type, &board->special[i].row, &board->special[i].column, &board->special[i].timer, &board->special[i].direction, &sprite);
            	board->special[i].original_timer = board->special[i].timer;
            	board->placed_piece[board->special[i].row][board->special[i].column] = sprite;
			}
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


BOARD (*TTO_FUNCTIONS_H_functions_sp[])(BOARD, int, int) = {move_pursuer, move_pman, activate_bomb, transmute, activate_magnet};


int game_loop (BOARD *board, int activate, int last_lvl) {
	int i, activated = 0, lvl_ended = 0, send_back = 0;
	if (last_lvl <= MAX_LVL) {
		if (winner(*board) || !board->free_slots){
			board->state = LOST;
			draw_board(*board, 0);
			printf("\n");
			load_lvl("lvls.txt", board->lvl, board);
			return 0;	
		}	
	}	
	if (activate) {
		for (i = 0; i < board->special_count; i++) {
			if (board->special[i].type < GREEN_SLOT) {
				activated = 1;
				*board = TTO_FUNCTIONS_H_functions_sp[board->special[i].type](*board, board->special[i].row, board->special[i].column);	
			}
		}
		if (board->conditions.swap > 2) {
			*board = swap_all(*board);
			activated = 1;	
		} 
		if (activated) {
			if (winner(*board)) lvl_ended = 1;
		}			
	}
	if (!lvl_ended){
		char move[INPUT_SIZE];
		draw_board(*board, 0);
		fgets(move, INPUT_SIZE, stdin);
		strupr(move);
		send_back = play(move, board);
		board->free_slots = update_free_slots(*board);
	}
	return send_back;
}


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


void update_lvls(int lvls[MAX_LVL], int quadrants[N_QUADRANTS], int lvl, int state) {
	int quadrant = 1, i;
	while (N_QUADRANTS * quadrant < lvl) {
		quadrant++;
	}
	quadrants[quadrant - 1] = state; 
	quadrant *= N_QUADRANTS;
	for (i = quadrant - N_QUADRANTS; i < quadrant; i++) {
		lvls[i] = state;
	}
}


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


int map(int lvl, int lvls[MAX_LVL], int call_menu) {
	int i, output = 1, control = 1, choice;
	printf("\n");
	for (i = 1; i <= MAX_LVL; i++) {
		if (lvls[output - 1] == LVL_COMPLETED) printf(ANSI_GREEN);	
		else if (lvls[output - 1] == LVL_PLAYING) printf(ANSI_YELLOW);
		else if (lvls[output - 1] == LVL_RED) printf(ANSI_RED);
		else printf(RESET_TEXT_FORMAT);
		if (output < 10) printf ("%1c%1c%1c", '0', output + '0', ' ');
		else printf("%1c%1c%1c", output / 10 + '0', output % 10 + '0', ' ');
		output += 1;
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
	if (lvl >= MAX_LVL) {
		printf(RESET_TEXT_FORMAT);
		if (lvl > MAX_LVL && call_menu) {
			printf("\nlvl %d?\n", MAX_LVL + 1);
			printf(ANSI_GREEN);
			printf("red\n");
			printf(RESET_TEXT_FORMAT);
		}	
	}
	if (call_menu) choice = map_menu(lvl);
	return choice;
}


int menu(int lvl) {
	int choice, max_choice = 2;
	char msg[40] = "1: continue\n2: new game\n0: exit";
	if (lvl == 1) {
		strcpy(msg, "1: new game\n0: exit");
		max_choice = 1;
	}
	do {
		printf("%s\n\n", msg);
		scanf("%d", &choice);
		scanf("%*[^\n]");
		scanf("%*c");	
		if (lvl > 1 && choice == 2) {
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
