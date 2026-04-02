#ifndef PLUG_H
#define PLUG_H

#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
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

enum texture_enum {
	e_BookShelf,
};

typedef struct
{
	Vector2 pos;
	Vector2 dir;
	float angle;
	uint8_t grid_x;
	uint8_t grid_y;
} Player;

typedef struct {
	uint8_t map[512];
	uint8_t map_width;
	uint8_t map_height;
	Player player;
} Map;

typedef struct {
	Texture2D canvas; // canvas buffer to call DrawTexture on
	Image image; // image buffer
	Texture2D wall;
	Map maps[MAP_COUNT];
	int current_map_index;
} GameState;

typedef enum {
	SPACE,
	WALL,
	ENEMY
} CellType ;
#endif // !PLUG_H
