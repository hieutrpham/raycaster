#include <raylib.h>
#include <stdint.h>
#define WIDTH 800
#define HEIGHT 800
#define BACKGROUND 0x202020FF

typedef struct
{
	float x;
	float y;
} player_t;

void update_player(player_t *player)
{
		if (IsKeyDown(KEY_W))
			player->y -= 5;
		if (IsKeyDown(KEY_S))
			player->y += 5;
		if (IsKeyDown(KEY_A))
			player->x -= 5;
		if (IsKeyDown(KEY_D))
			player->x += 5;
		DrawRectangle(player->x, player->y, 20, 20, RED);
}

int mapX=8, mapY = 8, mapS=64;
int map[] =
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
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
		}
	}
}

int main(void)
{
	InitWindow(WIDTH, HEIGHT, "Raylib Template");
	SetTargetFPS(60);
	player_t player = {.x = WIDTH/2, .y = HEIGHT/2};

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
