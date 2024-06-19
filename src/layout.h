#ifndef LAYOUT_H_
#define LAYOUT_H_

#include "MGF.h"

void drawPlayField(SDL_Texture* background, unsigned short size, int XOffset);
void drawScoreBox(SDL_Texture* background, unsigned short size, bool inCustomMode, int XOffset, bool inMultiplayer);
void drawLevelBox(SDL_Texture* background, unsigned short size, int XOffset);
void drawUntilBox(SDL_Texture* background, unsigned short size, int XOffset);
void drawNextBox(SDL_Texture* background, unsigned short size, int XOffset);
void drawHoldBox(SDL_Texture* background, unsigned short size, int XOffset);
void drawSizeBagBox(SDL_Texture* background, unsigned short size, int XOffset);
int calcMapWidth();
int calcMapHeight();
SDL_Texture* create_Score_Text();
SDL_Texture* create_Name_Text();
SDL_Texture* create_Level_Text();
SDL_Texture* create_Lines_Text();
SDL_Texture* create_Pause_Text();
SDL_Texture* create_Foreground_Text();
SDL_Texture* create_Cursor_Text();
SDL_Texture* create_volSlide_Text();
void updateValuesText(SDL_Texture* texture);
SDL_Texture* create_Values_Text();
void updateControlsText(SDL_Texture* texture, int selected_control, bool editing);
void updateCustomText(SDL_Texture* texture, unsigned short value);
char* createSizeBagStringOne();
char* createSizeBagStringTwo();
SDL_Texture* create_SizeBag_Text();
SDL_Texture* create_Custom_Text();
SDL_Texture* create_Controls_Text();
UI_list* _create_list(unsigned short color, const char* strings, ...);
UI_list* create_Modes_List();
UI_list* create_Numerical_List();
UI_list* create_Custom_List();
UI_list* create_Options_List();
float getSizeBagMulti(unsigned short size);
int getSizeBagX(unsigned short size, float multiplier);
int getSizeBagY(unsigned short size);
int getListEntryY(UI_list* list, const char* str);
const char* getListSelectedString(UI_list* list);
int getListSelectedEntryY(UI_list* list);
void delete_UI_list(UI_list** list);
piece** makeTitlePieces();
SDL_Texture* create_Title_Text();
piece** getMovingPieces(piece** titlePieces);
bool updateTitle(SDL_Texture* texture, piece** movingPieces);
int getScoreDrawX(unsigned short size);
int getScoreDrawY(unsigned short size);
int getForegroundX(unsigned short size);
int getForegroundY(unsigned short size);
int getLevelX(unsigned short size, unsigned short level);
int getLevelY(unsigned short size);
int getLinesX(unsigned short size, unsigned short lines);
int getLinesY(unsigned short size);
int getNextX(unsigned short size, int width);
int getNextY(unsigned short size, int height);
int getHoldX(unsigned short size, int width);
int getHoldY(unsigned short size, int height);
void drawBackgroundExtras(SDL_Texture* background, unsigned short size);
int getGameWidth(unsigned short size, bool multiplayer);
int getGameHeight(unsigned short size);
int getPausedX(unsigned short size, float multi);
int getPausedY(unsigned short size, float multi);
void updatePausedText(SDL_Texture* texture, bool exit);
UI_list* create_Connect_List();
SDL_Texture* create_ConnectionValues_Text();
void updateConnectionValuesText(SDL_Texture* texture, char* ip, char* port, char* name);
SDL_Texture* create_ConnectionMessage_Text();
void updateConnectionMessageText(SDL_Texture** texture, char* message);
int getConnectionMessageX(char* message, float multi);
int getConnectionMessageY(float multi);
int getNameDrawX(unsigned short size);
int getNameDrawY(unsigned short size);

#endif