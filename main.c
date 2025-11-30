#include "raylib.h"
#include "raymath.h"
#include <stdint.h>
#include <math.h>
#include <stdio.h>

#define player_size 20
#define WIDTH 800
#define SCENE_WIDTH 1600
#define HEIGHT 800
#define mapX 8
#define mapY 8
#define DR (PI / 180.0f)
#define BACKGROUND 0x101010FF
#define PLAYER_ANGLE 10
#define cell_size (WIDTH/mapX)
#define PROJECTION_DIST 277.0f
#define WALL_HEIGHT 692.8f
#define FOV 60.0f

int map[] =
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,1,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,1,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

typedef struct
{
	float x;
	float y;
	float dx;
	float dy;
	float angle;
} player_t;

void drawRays(player_t *p)
{
	int yo, xo;
	float ra = p->angle - 30.0f * DR;
	// TraceLog(LOG_INFO, "player angle: %f", p->angle);
	for (int r = 0; r < 60; r++, ra += DR)
	{
		int dof = 0;
		float aTan = -1/tanf(ra);
		float hx = p->x, hy = p->y;
		if (ra < 0)
			ra += 2*PI;
		if (ra > 2*PI)
			ra -= 2*PI;
		// float distH = 1e9;
		// NOTE: find horizontal intersection
		// looking up
		if (ra > PI) {
			hy = floorf(p->y / cell_size) * cell_size - 0.001f;
			hx = (p->y - hy)*aTan + p->x;
			yo = -cell_size;
			xo = -yo*aTan;
		}
		// looking down
		else if (ra < PI && ra > 0) {
			hy = floorf(p->y / cell_size) * cell_size + cell_size;
			hx = (p->y - hy)*aTan + p->x;
			yo = cell_size;
			xo = -yo*aTan;
		}
		else {
			hx = p->x;
			hy = p->y;
			dof = mapX;
		}
		while (dof < mapX)
		{
			int mx = (int)(hx/cell_size);
			int my = (int)(hy/cell_size);
			int mp = my * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1)
				dof = mapX;
			else
			{
				hx += xo;
				hy += yo;
				dof++;
			}
		}
		// NOTE: find vertical intersection
		aTan = -tanf(ra);
		dof = 0;
		float vx = p->x, vy = p->y;
		// looking right
		if ((ra > 3*PI/2 && ra < 2*PI) || (ra < PI/2 && ra > 0))
		{
			vx = floorf(p->x / cell_size) * cell_size + cell_size;
			vy = p->y + (p->x - vx)*aTan;
			xo = cell_size;
			yo = -xo*aTan;
		}
		// looking left
		else if (ra > PI/2 && ra < 3*PI/2)
		{
			vx = floorf(p->x / cell_size) * cell_size - 0.001f;
			vy = p->y + (p->x - vx)*aTan;
			xo = -cell_size;
			yo = -xo*aTan;
		}
		else {
			vx = p->x;
			vy = p->y;
			dof = mapX;
		}
		while (dof < mapX)
		{
			int mx = (int)(vx/cell_size);
			int my = (int)(vy/cell_size);
			int mp = my * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1)
				dof = mapX;
			else
			{
				vx += xo;
				vy += yo;
				dof++;
			}
		}
		float distH = (hy - p->y)*(hy - p->y) + (hx - p->x)*(hx - p->x);
		float distV = (vy - p->y)*(vy - p->y) + (vx - p->x)*(vx - p->x);
		float dist = distV > distH ? distH : distV;
		dist = sqrtf(dist);
		float corrected_dist = dist * cosf(ra - p->angle);
#if 1
		if ( distH > distV)
			DrawLine(p->x + 10, p->y + 10, vx, vy, RED);
		else
			DrawLine(p->x + 10, p->y + 10, hx, hy, RED);
#endif
		// NOTE: draw wall
		float lineH = (cell_size * WALL_HEIGHT)/corrected_dist;
		if (lineH > HEIGHT) lineH = HEIGHT;
		float line_offset = (HEIGHT/2.0f) - (lineH/2.0f);
		DrawRectangle(800 + r*(800)/FOV, line_offset, 800/FOV, lineH, GREEN);
	}
}

void update_player(player_t *player)
{
	if (IsKeyDown(KEY_W))
	{
		player->x += player->dx;
		player->y += player->dy;
	}
	if (IsKeyDown(KEY_S))
	{
		player->x -= player->dx;
		player->y -= player->dy;
	}
	if (IsKeyDown(KEY_A))
	{
		player->angle -= 0.1f;
		if (player->angle < 0)
			player->angle += 2*PI;
		player->dx = cosf(player->angle) * PLAYER_ANGLE;
		player->dy = sinf(player->angle) * PLAYER_ANGLE;
	}
	if (IsKeyDown(KEY_D))
	{
		player->angle += 0.1f;
		if (player->angle > 2*PI)
			player->angle -= 2*PI;
		player->dx = cosf(player->angle) * PLAYER_ANGLE;
		player->dy = sinf(player->angle) * PLAYER_ANGLE;
	}
	DrawRectangle(player->x, player->y, player_size, player_size, RED);
	// DrawLine(player->x, player->y, player->x + PLAYER_ANGLE*player->dx, player->y + PLAYER_ANGLE*player->dy, GREEN);
}

void drawMap()
{
	for (int y = 0; y < mapY; y++)
	{
		for (int x = 0; x < mapX; x++)
		{
			if (map[y*mapX + x] == 1)
				DrawRectangle(x * WIDTH/mapX, y * HEIGHT/mapY, WIDTH/mapX, HEIGHT/mapY, YELLOW);
			DrawLine(x * WIDTH/mapX, 0, x * WIDTH/mapX, HEIGHT, GetColor(0x202020FF));
			DrawLine(0, y * HEIGHT/mapY, WIDTH, y * HEIGHT/mapY, GetColor(0x202020FF));
		}
	}
}

int main(void)
{
	InitWindow(SCENE_WIDTH, HEIGHT, "Raycaster");
	SetTargetFPS(60);
	player_t player = {
		.x = WIDTH/2 + cell_size/2 - player_size/2,
		.y = HEIGHT/2 + cell_size/2 - player_size/2,
		.angle = PI/2,
		.dx = cos(PI/2),
		.dy = sin(PI/2)
	};

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(GetColor(BACKGROUND));
			drawMap();
			update_player(&player);
			drawRays(&player);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
