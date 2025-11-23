#include <raylib.h>
#include <raymath.h>
#include <stdint.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 800
#define BACKGROUND 0x101010FF
#define PLAYER_ANGLE 10

typedef struct
{
	float x;
	float y;
	float dx;
	float dy;
	float angle;
} player_t;

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
		DrawRectangle(player->x, player->y, 20, 20, RED);
		DrawLine(player->x, player->y, player->x + PLAYER_ANGLE*player->dx, player->y + PLAYER_ANGLE*player->dy, GREEN);
}

int mapX=8, mapY = 8, mapS=64;
int map[] =
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,1,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

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
	InitWindow(WIDTH, HEIGHT, "Raycaster");
	SetTargetFPS(60);
	player_t player = {.x = WIDTH/2, .y = HEIGHT/2, .angle = PI/2, .dx = cos(PI/2), .dy = sin(PI/2)};

	while (!WindowShouldClose()) {
		BeginDrawing();
			ClearBackground(GetColor(BACKGROUND));
			drawMap();
			update_player(&player);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
