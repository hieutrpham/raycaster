#include <dlfcn.h>
#include <stdio.h>
#include "raylib.h"

#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#define mapX 8
#define mapY 8
#define DR 0.017453292519943295f
#define BACKGROUND 0x101010FF
#define PROJECTION_DIST 1662.0f
#define FOV 60.0f
#define SPEED 5.f
#define MAP_COUNT 2

typedef struct
{
	Vector2 pos;
	Vector2 dir;
	float angle;
} Player;

typedef struct {
	int map[512];
	int map_width;
	int map_height;
} Map;

typedef struct {
	Player p;
	Map maps[MAP_COUNT];
	int current_map_index;
} GameState;
