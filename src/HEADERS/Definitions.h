#define FONT_WIDTH 	12
#define FONT_HEIGHT 12
#define STRING_GAP	2

#define SPRITE_WIDTH  12
#define SPRITE_HEIGHT 12

#define INITIAL_INTERNAL_WIDTH	600
#define INITIAL_INTERNAL_HEIGHT	600

#define BLOCK_SPRITE_ID_1 	0
#define BLOCK_SPRITE_ID_2	1
#define WALL_SPRITE_ID		2

#define NUM_OF_CONTROLS			10
#define LEFT_BUTTON_ID			0
#define RIGHT_BUTTON_ID			1
#define SOFT_DROP_BUTTON_ID		2
#define HARD_DROP_BUTTON_ID		3
#define HOLD_BUTTON_ID			4
#define ROTATE_CCW_BUTTON_ID	5
#define ROTATE_CW_BUTTON_ID		6
#define MIRROR_BUTTON_ID		7
#define SELECT_BUTTON_ID		8
#define EXIT_BUTTON_ID			9

#define NUM_OF_SOUNDS		7
#define COMPLETE_SOUND_ID 	0
#define HOLD_SOUND_ID		1
#define LAND_SOUND_ID		2
#define MOVE_SOUND_ID		3
#define OVER_SOUND_ID		4
#define PAUSE_SOUND_ID		5
#define ROTATE_SOUND_ID		6

//Speed is in cells per second
		//Calculated using values from NES version of Tetris
#define INITIAL_SPEED	(double)(60.0988 / 48)

#define ASPECT_RATIO				1
#define HEIGHT_IN_CHARS				50
#define WIDTH_OF_PLAYSPACE			34
#define WINDOW_BOX_DIMENSION		600

#define SPACE_CHAR		26
#define ZERO_CHAR		27
#define COLON_CHAR		39
#define WALL_CHAR		37
#define RIGHT_CHAR		40
#define LEFT_CHAR		41
#define UP_CHAR			42

#define GRAY	0
#define WHITE	1
#define YELLOW	2
#define GREEN	3
#define CYAN	4
#define MAGENTA	5
#define ORANGE	6
#define BLUE	7
#define RED		8
#define BLACK	9

#define TITLE_SCREEN	0
#define PLAY_SCREEN		1
#define CONTROLS_SCREEN	2
#define RESET			3

#define MAX_PIECE_SIZE		9
#define NUM_OF_TITLE_PIECES 19

#define NONE	0
#define UP		0
#define RIGHT	1
#define DOWN	2
#define LEFT	3

#define CW	0
#define CCW	1

#define CLEARING_TIME	20
#define OVER_TIME		42

#define MOVEMENT_WAIT	500
#define MOVEMENT_TIME	50

#define HOLD_TEXTURE_MULTI 0.75

#define OPTIONS	10

#define WINDOWED	0
#define FULLSCREEN 	1

#define NUM_OF_OPTIONS 4