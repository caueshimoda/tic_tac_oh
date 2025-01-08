#ifndef TTO_STRUCTURES_H
#define TTO_STRUCTURES_H


enum matrix {QUADRANT_MATRIX = 3, MAX_MATRIX = 5, N_QUADRANTS = 9};
enum max_special {MAX_SP = 9};
enum color {STD_COLOR, GREEN, RED};
enum special_type {PURSUER, PMAN, BOMB, TRANSMUTER, MAGNET, GREEN_SLOT, RED_SLOT, NONE};
enum levels {LVL_NOT_PLAYED, LVL_PLAYING, LVL_COMPLETED, LVL_RED};


typedef struct {
	int type;
	int row;
	int column;
	int timer; // Bombs only.
	int original_timer; // Bombs only.
	int direction; // PMan only.
} SPECIAL;


typedef struct {
	int has_colored_slot;
	int swap;
	int summation;
	int sum[MAX_MATRIX * 2]; //Levels with summation only.
} CONDITIONS;


typedef struct {
	char placed_piece[MAX_MATRIX][MAX_MATRIX];
	CONDITIONS conditions;
	SPECIAL special[MAX_SP];
	int special_count;
	int matrix;
	int free_slots;
	char next;
	char target;
	char wrong;
	int lvl;
	int state;
} BOARD;


#endif // TTO_STRUCTURES_H
