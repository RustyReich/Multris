#ifndef FILE_H_
#define FILE_H_

#include "MGF.h"

unsigned short getLineCount(char* fileName);
bool fileExists(char* fileName);
void createFile(char* file_path);
void createProgressFile();
void createCustomTopFile();
void createTopFile();
void createOptions();
void createWindowFile();
void createControls();
char* getNameAtLine(const char* file_path, int line);
int getFileValue(const char* file_path, const char* name);
bool brokenProgress();
bool brokenOptions();
bool brokenControls();
bool brokenWindowFile();
void saveToFile(const char* file_path, const char* str, int value);
unsigned int loadTop(unsigned short size, bool inCustomMode);
unsigned int loadProgress();
void saveTop(unsigned int score, unsigned short size, bool inCustomMode);
void saveProgress();
void saveWindowSettings();
void createNameFile();
bool brokenNameFile();
void saveName(char* name);
char* loadName();
void createHostPortFile();
bool brokenHostPortFile();
void saveHostPort(char* portString);
char* loadHostPort();
void createIPFile();
bool brokenIPFile();
void saveIP(char* ipString);
char* loadIP();
char* loadConnectPort();
void saveConnectPort(char* portString);
bool brokenConnectPort();
void createConnectPortFile();

#endif