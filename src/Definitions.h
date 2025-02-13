#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include "MGF.h"

#define FONT_WIDTH 	12
#define FONT_HEIGHT 12
#define STRING_GAP	2

#define SPRITE_WIDTH  12
#define SPRITE_HEIGHT 12

#define MAX_PIECE_SIZE 8

#define OPTIONS	9

#define BASE_PLAYFIELD_WIDTH 	2.5
#define BASE_PLAYFIELD_HEIGHT	5

#define NUM_MOVING_TITLE_PIECES	14

#define MOVEMENT_WAIT	500
#define MOVEMENT_TIME	50

//Speed is in cells per second
		//Calculated using values from NES version of Tetris
#define INITIAL_SPEED	(double)(60.0988 / 48)

#define CLEARING_TIME	20

#define HOLD_TEXTURE_MULTI 0.75

#define MIN_CUSTOM_SIZE 4
#define MAX_CUSTOM_SIZE 15

#define LEVELUP_INCREASE 3

#define NUM_OF_OPTIONS 				8
#define NUM_OF_MULTIPLAYER_OPTIONS	3
#define NUM_OF_PAUSED_OPTIONS		7

#define MULTIPLAYER_PULSE_DELAY_SECONDS 1

#define MAX_NAME_LENGTH 7

#define SERVERMESSAGE_BUFFER_SIZE 		1024
#define CONNECTION_TIMEOUT_SECONDS		5
#define SERVER_TICK_RATE 				128
#define HOST_GAME_DATA_MAX_LENGTH		48
#define CONNECT_GAME_DATA_MAX_LENGTH	48

#define MUSIC_FADEOUT_MS 1000

#define FIRST_NUMERICAL_SIZE 4

enum Control_Types {
	
	LEFT_BUTTON,
	RIGHT_BUTTON,
	SOFT_DROP_BUTTON,
	HARD_DROP_BUTTON,
	HOLD_BUTTON,
	ROTATE_CCW_BUTTON,
	ROTATE_CW_BUTTON,
	MIRROR_BUTTON,
	SELECT_BUTTON,
	EXIT_BUTTON,
	DOWN_BUTTON,
	UP_BUTTON,
	NUM_OF_CONTROLS

};

enum Music_Types {

	INTRO_MUSIC_TRACK,
	LOOP_MUSIC_TRACK,
	NUM_OF_MUSIC_TRACKS

};

enum Sound_Types {

	COMPLETE_SOUND,
	HOLD_SOUND,
	LAND_SOUND,
	MOVE_SOUND,
	OVER_SOUND,
	PAUSE_SOUND,
	ROTATE_SOUND,
	UNLOCK_SOUND,
	NUM_OF_SOUNDS

};

enum Color_Types {

	GRAY = 1,
	WHITE = 2,
	YELLOW = 3,
	GREEN = 4,
	CYAN = 5,
	MAGENTA = 6,
	ORANGE = 7,
	BLUE = 8,
	RED = 9,
	BLACK = 10,
	DARK_GRAY = 11

};

enum Texture_Types {

	SCORE_TEXT,
	LEVEL_TEXT,
	LINES_TEXT,
	PAUSED_TEXT,
	FOREGROUND_TEXT,
	TITLE_TEXT,
	CURSOR_TEXT,
	VALUES_TEXT,
	VOLSLIDE_TEXT,
	CONTROLS_TEXT,
	CUSTOM_TEXT,
	SIZEBAG_TEXT,
	CONNECTIONVALUES_TEXT,
	CONNECTIONMESSAGE_TEXT,
	NAME_TEXT,
	HOSTINGVALUES_TEXT,
	PAUSEDVALUES_TEXT,
	NUM_OF_TEXTURES

};

enum List_Types {

	MODES_LIST,
	NUMERICAL_LIST,
	CUSTOM_LIST,
	OPTIONS_LIST,
	CONNECTION_LIST,
	MULTIPLAYER_LIST,
	HOSTING_LIST,
	PAUSE_LIST,
	NUM_OF_LISTS

};

enum Data_Type {

	TYPE_NA,
	VARIABLE,
	PIECE,
	TEXTURE,
	MOVING_TITLE_PIECES,
	UI_LIST,
	SIZE_BAG,
	NUM_OF_DATA_TYPES

};

enum Screen_Type {

	TITLE_SCREEN,
	PLAY_SCREEN,
	CONTROLS_SCREEN,
	MULTIPLAYERLOBBY_SCREEN,
	RESET,
	NUM_OF_SCREENS

};

enum Rotation_Type {

	CW,
	CCW,
	NUM_OF_ROTATIONS

};

enum Direction_Type {

	UP,
	RIGHT,
	DOWN,
	LEFT,
	NONE,
	NUM_OF_DIRECTIONS

};

#endif