#include <dlfcn.h>
#include <stdio.h>
#include "raylib.h"

typedef struct
{
	float x;
	float y;
	float dx;
	float dy;
	float angle;
} Player;

