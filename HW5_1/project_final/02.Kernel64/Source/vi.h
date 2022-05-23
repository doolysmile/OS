#ifndef __VI_H__
#define __VI_H__

#include "Console.h"
#include "FileSystem.h"
#include "Keyboard.h"

#define VI_READ_MODE	0
#define VI_WRITE_MODE	1
#define VI_COMMAND_MODE	2

#define VI_MAXBUFFERCOUNT	300
void kViEditor(int x, int y);
void kTest(int XX, int YY, char *vcFileName);

void kSaveFile(char *vcFileName);
#endif
