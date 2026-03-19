#include "raylib.h"
#include "plug.h"
#include "raymath.h"

int map[] =
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,1,0,0,1,
	1,0,1,0,1,0,0,1,
	1,0,0,0,1,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,1,0,0,1,0,1,
	1,0,1,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

static const float ray_delta = FOV * DR/CANVAS_WIDTH;

void drawRays(Player p)
{
	float step_y, step_x;
	float ra = p.angle - 30.0f * DR;

	for (int r = 0; r < CANVAS_WIDTH; r++, ra += ray_delta)
	{
		int dof = 0;
		float aTan = -1/tanf(ra);
		float hx, hy;
		if (ra < 0)
			ra += 2*PI;
		if (ra > 2*PI)
			ra -= 2*PI;
		// NOTE: find horizontal intersection
		// looking up
		if (ra > PI) {
			hy = floorf(p.pos.y) - 0.001f;
			hx = (p.pos.y - hy)*aTan + p.pos.x;
			step_y = -1.0f;
			step_x = aTan;
		}
		// looking down
		else if (ra < PI && ra > 0) {
			hy = floorf(p.pos.y) + 1.0f;
			hx = (p.pos.y - hy)*aTan + p.pos.x;
			step_y = 1.0f;
			step_x = -aTan;
		}
		else {
			hx = p.pos.x;
			hy = p.pos.y;
			dof = mapX;
		}
		while (dof < mapX)
		{
			int mx = (int)(hx);
			int my = (int)(hy);
			int mp = my * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1)
				dof = mapX;
			else
			{
				hx += step_x;
				hy += step_y;
				dof++;
			}
		}
		// NOTE: find vertical intersection
		aTan = -tanf(ra);
		dof = 0;
		float vx = p.pos.x, vy = p.pos.y;
		// looking right
		if ((ra > 3*PI/2 && ra < 2*PI) || (ra < PI/2 && ra > 0))
		{
			vx = floorf(p.pos.x) + 1.0f;
			vy = p.pos.y + (p.pos.x - vx)*aTan;
			step_x = 1.0f;
			step_y = -aTan;
		}
		// looking left
		else if (ra > PI/2 && ra < 3*PI/2)
		{
			vx = floorf(p.pos.x) - 0.001f;
			vy = p.pos.y + (p.pos.x - vx)*aTan;
			step_x = -1.0f;
			step_y = aTan;
		}
		else {
			vx = p.pos.x;
			vy = p.pos.y;
			dof = mapX;
		}
		while (dof < mapX)
		{
			int mx = (int)(vx);
			int my = (int)(vy);
			int mp = my * mapX + mx;
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1)
				dof = mapX;
			else
			{
				vx += step_x;
				vy += step_y;
				dof++;
			}
		}

		float distH = (hy - p.pos.y)*(hy - p.pos.y) + (hx - p.pos.x)*(hx - p.pos.x);
		float distV = (vy - p.pos.y)*(vy - p.pos.y) + (vx - p.pos.x)*(vx - p.pos.x);
		float dist = distV > distH ? distH : distV;
		dist = sqrtf(dist);
		float corrected_dist = dist * cosf(ra - p.angle);

		// NOTE: draw wall
		float wall_height = PROJECTION_DIST/corrected_dist;
		if (wall_height > CANVAS_HEIGHT)
			wall_height = CANVAS_HEIGHT;
		float line_offset = (CANVAS_HEIGHT - wall_height)/2.0f;
		DrawRectangle(r, (int)line_offset, 1, (int)wall_height, SKYBLUE);
	}
}

static bool in_bound (Vector2 pos, GameState *game) {
	(void)pos;
	(void)game;
	return true;
}

static void update_player(GameState *game) {
	Player *player = &game->p;
	int fps = GetFPS();
	if (IsKeyDown(KEY_W)) {
		Vector2 new_pos = Vector2Add(player->pos, Vector2Scale(player->dir, SPEED/fps));
		if (in_bound(new_pos, game))
			player->pos = new_pos;
	}
	if (IsKeyDown(KEY_S)) {
		player->pos = Vector2Subtract(player->pos, Vector2Scale(player->dir, SPEED/fps));
	}
	if (IsKeyDown(KEY_A)) {
		Vector2 new_dir = {player->dir.y, -player->dir.x};
		player->pos = Vector2Add(Vector2Scale(new_dir, SPEED/fps), player->pos);
	}
	if (IsKeyDown(KEY_D)) {
		Vector2 new_dir = {-player->dir.y, player->dir.x};
		player->pos = Vector2Add(Vector2Scale(new_dir, SPEED/fps), player->pos);
	}
}

void render(GameState *game) {
	BeginDrawing();
	ClearBackground(GetColor(BACKGROUND));
	update_player(game);
	drawRays(game->p);
	DrawFPS(0, 0);
	EndDrawing();
}
