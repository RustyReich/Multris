#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define FONT_WIDTH 	12
#define FONT_HEIGHT 12
#define STRING_GAP	2

#define SPRITE_WIDTH  12
#define SPRITE_HEIGHT 12

#define BLOCK_SPRITE_ID 	1
#define WALL_SPRITE_ID		2

#define NUM_OF_CONTROLS		12
#define LEFT_BUTTON			0
#define RIGHT_BUTTON		1
#define SOFT_DROP_BUTTON	2
#define HARD_DROP_BUTTON	3
#define HOLD_BUTTON			4
#define ROTATE_CCW_BUTTON	5
#define ROTATE_CW_BUTTON	6
#define MIRROR_BUTTON		7
#define SELECT_BUTTON		8
#define EXIT_BUTTON			9
#define DOWN_BUTTON			10
#define UP_BUTTON			11

#define NUM_OF_SOUNDS	8
#define COMPLETE_SOUND 	0
#define HOLD_SOUND		1
#define LAND_SOUND		2
#define MOVE_SOUND		3
#define OVER_SOUND		4
#define PAUSE_SOUND		5
#define ROTATE_SOUND	6
#define UNLOCK_SOUND	7

#define GRAY		0
#define WHITE		1
#define YELLOW		2
#define GREEN		3
#define CYAN		4
#define MAGENTA		5
#define ORANGE		6
#define BLUE		7
#define RED			8
#define BLACK		9
#define DARK_GRAY 	10

#define MAX_PIECE_SIZE 8

#define OPTIONS	9

#define BASE_PLAYFIELD_WIDTH 	2.5
#define BASE_PLAYFIELD_HEIGHT	5

#define SCORE_TEXT		0
#define LEVEL_TEXT		1
#define LINES_TEXT		2
#define PAUSED_TEXT		3
#define FOREGROUND_TEXT	4
#define TITLE_TEXT		5
#define CURSOR_TEXT		6
#define VALUES_TEXT		7
#define VOLSLIDE_TEXT	8
#define CONTROLS_TEXT	9
#define CUSTOM_TEXT		10
#define SIZEBAG_TEXT	11

#define MODES_LIST		0
#define NUMERICAL_LIST	1
#define CUSTOM_LIST		2
#define OPTIONS_LIST	3

#define OVER_SOUND_LENGTH 2000

#define NUM_MOVING_TITLE_PIECES	14

#define TYPE_NA		        0
#define VARIABLE	        1
#define PIECE		        2
#define TEXTURE             3
#define MOVING_TITLE_PIECES 4
#define UI_LIST				5
#define SIZE_BAG			6

#define MOVEMENT_WAIT	500
#define MOVEMENT_TIME	50

//Speed is in cells per second
		//Calculated using values from NES version of Tetris
#define INITIAL_SPEED	(double)(60.0988 / 48)

#define TITLE_SCREEN	0
#define PLAY_SCREEN		1
#define CONTROLS_SCREEN	2
#define RESET			3

#define NONE	0
#define UP		0
#define RIGHT	1
#define DOWN	2
#define LEFT	3

#define CW	0
#define CCW	1

#define CLEARING_TIME	20

#define HOLD_TEXTURE_MULTI 0.75

#define MAX_CUSTOM_SIZE 15

#define LEVELUP_INCREASE 3

#endif