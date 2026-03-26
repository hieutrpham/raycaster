#include "raylib.h"
#include "plug.h"
#include "raymath.h"
#include <math.h>
#include <stdint.h>
#include <string.h>

static const float ray_delta = FOV * DR/CANVAS_WIDTH;

static bool hit_wall(int pos, Map current_map) {
	return (pos > 0 && pos < current_map.map_width * current_map.map_height && current_map.map[pos] == 1);
}

void drawRays(GameState *game)
{
	Player *p = &game->player;
	Map current_map = game->maps[game->current_map_index];
	uint32_t *image_data = (uint32_t *)game->image.data; // PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 32 bpp
	bzero(image_data, game->image.width * game->image.height * 4);
	// for (int y = 0; y < game->image.height; ++y) {
	// 	for (int x = 0; x < game->image.width; ++x) {
	// 		image_data[y * game->image.width + x] = 0x181818ff;
	// 	}
	// }
	float step_y, step_x;
	float ra = p->angle - 30.0f * DR;

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
			hy = floorf(p->pos.y) - 0.001f;
			hx = (p->pos.y - hy)*aTan + p->pos.x;
			step_y = -1.0f;
			step_x = aTan;
		}
		// looking down
		else if (ra < PI && ra > 0) {
			hy = floorf(p->pos.y) + 1.0f;
			hx = (p->pos.y - hy)*aTan + p->pos.x;
			step_y = 1.0f;
			step_x = -aTan;
		}
		else {
			hx = p->pos.x;
			hy = p->pos.y;
			dof = current_map.map_width;
		}
		while (dof < current_map.map_width)
		{
			int mx = (int)(hx);
			int my = (int)(hy);
			int mp = my * current_map.map_width + mx;
			if (hit_wall(mp, current_map))
				dof = current_map.map_width;
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
		float vx = p->pos.x, vy = p->pos.y;
		// looking right
		if ((ra > 3*PI/2 && ra < 2*PI) || (ra < PI/2 && ra > 0))
		{
			vx = floorf(p->pos.x) + 1.0f;
			vy = p->pos.y + (p->pos.x - vx)*aTan;
			step_x = 1.0f;
			step_y = -aTan;
		}
		// looking left
		else if (ra > PI/2 && ra < 3*PI/2)
		{
			vx = floorf(p->pos.x) - 0.001f;
			vy = p->pos.y + (p->pos.x - vx)*aTan;
			step_x = -1.0f;
			step_y = aTan;
		}
		else {
			vx = p->pos.x;
			vy = p->pos.y;
			dof = current_map.map_width;
		}
		while (dof < current_map.map_width)
		{
			int mx = (int)(vx);
			int my = (int)(vy);
			int mp = my * current_map.map_width + mx;
			if (hit_wall(mp, current_map))
				dof = current_map.map_width;
			else
			{
				vx += step_x;
				vy += step_y;
				dof++;
			}
		}

		float distH = (hy - p->pos.y)*(hy - p->pos.y) + (hx - p->pos.x)*(hx - p->pos.x);
		float distV = (vy - p->pos.y)*(vy - p->pos.y) + (vx - p->pos.x)*(vx - p->pos.x);
		float dist = distV > distH ? distH : distV;
		dist = sqrtf(dist);
		float corrected_dist = dist * cosf(ra - p->angle);

		// NOTE: draw wall
		float wall_height = PROJECTION_DIST/corrected_dist;
		if (wall_height > CANVAS_HEIGHT)
			wall_height = CANVAS_HEIGHT;
		float line_offset = (CANVAS_HEIGHT - wall_height)/2.0f;

		// if ray hit horizontal line, texture x will be the mod of hx else vy;
		float tx = distH < distV ? fmod(hx, 1.f) : fmod(vy, 1.f);
		Rectangle src_rec = {tx * game->wall.width, 0, 0, game->wall.height };
		Rectangle dest_rec = {(float)r, line_offset, 1, (int)wall_height};
		float brightness = corrected_dist*10.f;
		Color tint = {WHITE.r * brightness, WHITE.g * brightness, WHITE.g * brightness, WHITE.a * brightness};
		DrawTexturePro(game->wall, src_rec, dest_rec, (Vector2){0, 0}, 0.f, tint);
		// DrawRectangle(r, (int)line_offset, 1, (int)wall_height, SKYBLUE);
		// for (int y = (int)line_offset; y < (int)line_offset + (int)wall_height; ++y) {
		// 	image_data[y * game->image.width + r] = 0xff181818;
		// }
	}
}

// check if the position is within the bounds of the map
// return: true if in bound, false if not
static bool is_wall (Vector2 pos, GameState *game) {
	Map map = game->maps[game->current_map_index];
	int mp = (int)pos.y * map.map_width + (int)pos.x;
	if (mp > 0 && mp < map.map_width * map.map_height && map.map[mp] == 1)
		return true;
	return false;
}

static void update_player(GameState *game) {
	Player *player = &game->player;
	int fps = GetFPS();
	if (IsKeyDown(KEY_W)) {
		Vector2 new_pos = Vector2Add(player->pos, Vector2Scale(player->dir, SPEED/fps));
		if (!is_wall(new_pos, game))
			player->pos = new_pos;
	}
	if (IsKeyDown(KEY_S)) {
		Vector2 new_pos = Vector2Subtract(player->pos, Vector2Scale(player->dir, SPEED/fps));
		if (!is_wall(new_pos, game))
			player->pos = new_pos;
	}
	if (IsKeyDown(KEY_A)) {
		Vector2 new_dir = {player->dir.y, -player->dir.x};
		Vector2 new_pos = Vector2Add(Vector2Scale(new_dir, SPEED/fps), player->pos);
		if (!is_wall(new_pos, game))
			player->pos = new_pos;
	}
	if (IsKeyDown(KEY_D)) {
		Vector2 new_dir = {-player->dir.y, player->dir.x};
		Vector2 new_pos = Vector2Add(Vector2Scale(new_dir, SPEED/fps), player->pos);
		if (!is_wall(new_pos, game)) {
			player->pos = new_pos;
		}
	}
}

void mouse_control(GameState *game) {
	Player *p = &game->player;
	int delta_x = GetMouseX() - CANVAS_WIDTH/2;
	p->angle += delta_x * MOUSE_SENSITIVITY;
	if (p->angle < 0)
		p->angle += 2 * PI;
	if (p->angle > 2 * PI)
		p->angle -= 2 * PI;
	p->dir.x = cosf(p->angle);
	p->dir.y = sinf(p->angle);
	SetMousePosition(CANVAS_WIDTH/2, CANVAS_HEIGHT/2);
}

void reset_pos(GameState *game) {
	Player *p = &game->player;
	Map map = game->maps[game->current_map_index];
	if (IsKeyPressed(KEY_Y)) {
		p->pos = (Vector2){map.map_width/2, map.map_height/2};
	}
}

void next_map(GameState *game) {
	int next_map = (game->current_map_index + 1) % MAP_COUNT;
	if (IsKeyPressed(KEY_M))
		game->current_map_index = next_map;
}

void render(GameState *game) {
	mouse_control(game);
	reset_pos(game);
	next_map(game);
	update_player(game);
	drawRays(game);
	UpdateTexture(game->canvas, game->image.data);
	BeginDrawing();
		ClearBackground(GetColor(BACKGROUND));
		DrawTexture(game->canvas, 0, 0, WHITE);
		DrawFPS(10, 10);
	EndDrawing();
}
