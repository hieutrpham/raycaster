#ifndef PLUG_H
#define PLUG_H

#include <stdlib.h>
#include "raymath.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dlfcn.h>
#include "raylib.h"
#include <math.h>

#define MOUSE_SENSITIVITY 0.001f
#define CANVAS_WIDTH 1920
#define CANVAS_HEIGHT 1080
#define DR 0.017453292519943295f
#define BACKGROUND 0x101010FF
#define PROJECTION_DIST 1662.0f
#define FOV 60.0f
#define SPEED 3.f
#define MAP_COUNT 2
#define IMAGE_COUNT 1
#define MAX_ARRAY_COUNT 32

enum texture_enum {
	MAP_0_TEXTURE,
};

typedef struct
{
	Vector2 pos;
	Vector2 dir;
	float angle;
	bool has_moved;
} Player;

typedef struct {
	Texture2D wall_texture;
	uint8_t map[512];
	uint8_t map_width;
	uint8_t map_height;
	Player player;
} Map;

typedef struct {
	Map maps[MAP_COUNT];
	int current_map_index;
	bool game_over;
} GameState;

typedef enum {
	SPACE,
	WALL,
	ENEMY,
	FRIEND,
} CellType ;

typedef struct {
	int value;
	int count;
} MemberInt;

typedef struct {
	MemberInt items[MAX_ARRAY_COUNT];
	int count;
} StaticArray;

#endif // !PLUG_H
