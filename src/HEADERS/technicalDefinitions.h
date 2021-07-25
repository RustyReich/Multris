//Global variables
extern unsigned short BLOCK_CHAR;
extern unsigned short UPDATE_FULLSCREEN_MODE;

#define CHAR_DIMENSION			12
#define ASPECT_RATIO			1
#define HEIGHT_IN_CHARS			50
#define WIDTH_OF_PLAYSPACE		34
#define WINDOW_BOX_DIMENSION	600

#define COMP_START	32
#define COMP_END	126

#define LETTER_GAP 2

#define SPACE_CHAR		26
#define ZERO_CHAR		27
#define COLON_CHAR		39
#define WALL_CHAR		37
#define BLOCK_CHAR_1	38
#define RIGHT_CHAR		40
#define LEFT_CHAR		41
#define UP_CHAR			42
#define BLOCK_CHAR_2	43

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
#define RESET			2

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

#define OPTIONS	10

#define FPS_MAX 500

#define WINDOWED	0
#define FULLSCREEN 	1

#define NUM_OF_OPTIONS 3

//Speed is in cells per second
		//Calculated using values from NES version of Tetris
#define INITIAL_SPEED	(double)(60.0988 / 48)