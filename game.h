#pragma once

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
#define MAP_COUNT 16
#define TEXTURE_COUNT 16
#define MAX_ARRAY_COUNT 32
#define ARRAY_LEN(a) (sizeof(a)/sizeof(a[0]))

enum texture_enum {
	MAP_0_TEXTURE,
	MAP_1_TEXTURE,
	ENEMY_TEXTURE,
	FRIEND_TEXTURE,
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

typedef enum {
	START_SCREEN,
	GAME_SCREEN,
	TEST_SCREEN,
	END_SCREEN,
	QUIT_GAME,
} GameScreen;

typedef struct {
	Map maps[MAP_COUNT];
	Texture2D enemy_texture;
	Texture2D friend_texture;
	int current_map_index;
	GameScreen screen_type;
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

void array_fill(Map *map, StaticArray *array, int value);
void draw_texture(Texture2D texture, float tx, float dest_x, float dest_y, float dest_height, Color color);
void interactive_button (GameState *game, GameScreen screen_type, Vector2 mouse_pos, Rectangle rec, const char *str);
void draw_text_center(const char* str, const int size, Color color);
