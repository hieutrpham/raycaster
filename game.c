#include "raylib.h"
#include <raymath.h>
#include "game.h"

static const float ray_delta = FOV * DR/CANVAS_WIDTH;

// used in gameplay logic
// check if the position is within the bounds of the map
// return: true if in bound, false if not
static bool is_wall (Vector2 pos, GameState *game) {
	Map map = game->maps[game->current_map_index];
	int mp = (int)pos.y * map.map_width + (int)pos.x;
	if (mp >= 0 && mp < map.map_width * map.map_height && (map.map[mp] == WALL || map.map[mp] == FRIEND))
		return true;
	return false;
}

// used in raycast rendering logic
static inline bool hit_wall(int pos, Map current_map) {
	return (pos >= 0 && pos < current_map.map_width * current_map.map_height
			&& current_map.map[pos] > SPACE && current_map.map[pos] < 0xff);
}

static inline int clamp_int(int value, int begin, int end) {
	int ret = value;
	if (ret <= begin)
		ret = begin;
	if (ret >= end)
		ret = end;
	return ret;
}

//:raycast algo
void raycast(GameState *game) {
	if (game->game_over)
		return;
	Player *p = &game->maps[game->current_map_index].player;
	Map current_map = game->maps[game->current_map_index];
	// uint32_t *image_data = (uint32_t *)game->image.data; // PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 32 bpp
	// bzero(image_data, game->image.width * game->image.height * 4);
	size_t mx, my, mph = 0, mpv = 0; // map coordinates
	float step_y, step_x; // DDA steps
	float ra = p->angle - 30.0f * DR; // FOV = 60
	size_t map_size = current_map.map_height * current_map.map_width;

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
			dof = current_map.map_height;
		}
		while (dof < current_map.map_height)
		{
			mx = (int)(hx);
			my = (int)(hy);
			mph = my * current_map.map_width + mx;
			if (hit_wall(mph, current_map))
				dof = current_map.map_height;
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
			mx = (int)(vx);
			my = (int)(vy);
			mpv = my * current_map.map_width + mx;
			if (hit_wall(mpv, current_map))
				dof = current_map.map_width;
			else
			{
				vx += step_x;
				vy += step_y;
				dof++;
			}
		}

		Vector2 horizonal_intersection = (Vector2){hx, hy};
		Vector2 vertical_intersection = (Vector2){vx, vy};
		float distH = Vector2LengthSqr(Vector2Subtract(horizonal_intersection, p->pos));
		float distV = Vector2LengthSqr(Vector2Subtract(vertical_intersection, p->pos));
		float dist = distV > distH ? distH : distV;
		float corrected_dist = sqrtf(dist) * cosf(ra - p->angle);

		// NOTE: draw wall
		float wall_height = PROJECTION_DIST/corrected_dist;
		float line_offset = (CANVAS_HEIGHT - wall_height)/2.0f;

		mph = clamp_int(mph, 0, map_size);
		mpv = clamp_int(mpv, 0, map_size);

		size_t mp_correct = distV > distH ? mph : mpv;

		float brightness = Clamp(wall_height/CANVAS_HEIGHT, 0.f, 1.f);
		// float brightness = 1.f;
		if (current_map.map[mp_correct] == WALL) {
			float tx = distH < distV ? fmod(hx, 1.f) : fmod(vy, 1.f);
			Rectangle src_rec = {tx * current_map.wall_texture.width, 0, 1, current_map.wall_texture.height };
			Rectangle dest_rec = {(float)r, line_offset, 1, (int)wall_height};
			Color tint = {WHITE.r * brightness, WHITE.g * brightness, WHITE.b * brightness, 255};
			DrawTexturePro(current_map.wall_texture, src_rec, dest_rec, (Vector2){0, 0}, 0.f, tint);
		} else if (current_map.map[mp_correct] == ENEMY) {
			DrawRectangle(r, (int)line_offset, 1, (int)wall_height, RED);
		} else {
			DrawRectangle(r, (int)line_offset, 1, (int)wall_height, GREEN);
		}
	}
}

static void update_player_pos(Vector2 new_pos, GameState *game) {
	Player *player = &game->maps[game->current_map_index].player;
	if (!is_wall(new_pos, game)) {
		if ((int)new_pos.y != (int)player->pos.y || (int)new_pos.x != (int)player->pos.x)
			player->has_moved = true;
		player->pos = new_pos;
	}
}
/* :player_update
 * */
static void update_player(GameState *game) {
	Player *player = &game->maps[game->current_map_index].player;
	float dt = GetFrameTime();
	if (IsKeyDown(KEY_W)) {
		Vector2 new_pos = Vector2Add(player->pos, Vector2Scale(player->dir, SPEED*dt));
		update_player_pos(new_pos, game);
	}
	if (IsKeyDown(KEY_S)) {
		Vector2 new_pos = Vector2Subtract(player->pos, Vector2Scale(player->dir, SPEED*dt));
		update_player_pos(new_pos, game);
	}
	if (IsKeyDown(KEY_A)) {
		Vector2 new_dir = {player->dir.y, -player->dir.x};
		Vector2 new_pos = Vector2Add(Vector2Scale(new_dir, SPEED*dt), player->pos);
		update_player_pos(new_pos, game);
	}
	if (IsKeyDown(KEY_D)) {
		Vector2 new_dir = {-player->dir.y, player->dir.x};
		Vector2 new_pos = Vector2Add(Vector2Scale(new_dir, SPEED*dt), player->pos);
		update_player_pos(new_pos, game);
	}
}

void mouse_control(GameState *game) {
	Player *p = &game->maps[game->current_map_index].player;
	int dist_x = GetMouseX() - CANVAS_WIDTH/2;
	p->angle += dist_x * MOUSE_SENSITIVITY;
	if (p->angle < 0)
		p->angle += 2 * PI;
	if (p->angle > 2 * PI)
		p->angle -= 2 * PI;
	p->dir.x = cosf(p->angle);
	p->dir.y = sinf(p->angle);
	SetMousePosition(CANVAS_WIDTH/2, CANVAS_HEIGHT/2);
}

/* :control player position and map index
*/
void control(GameState *game) {
	Player *p = &game->maps[game->current_map_index].player;
	Map map = game->maps[game->current_map_index];
	int next_map = (game->current_map_index + 1) % MAP_COUNT;
	if (IsKeyPressed(KEY_Y)) // reset player position
		p->pos = (Vector2){map.map_width/2, map.map_height/2};
	if (IsKeyPressed(KEY_M)) // change map
		game->current_map_index = next_map;
}

//:enemy_update
void enemy_update(GameState *game) {
	Map *current_map = &game->maps[game->current_map_index];
	if (!current_map)
		return;
	if (!current_map->player.has_moved)
		return;
	uint8_t *map = game->maps[game->current_map_index].map;
	StaticArray enemy_new_pos = {0};
	StaticArray enemy_old_pos = {0};
	int player_pos_x = (int)current_map->player.pos.x;
	int player_pos_y = (int)current_map->player.pos.y;
	for (int y = 0; y < current_map->map_height; ++y) {
		for (int x = 0; x < current_map->map_width; ++x) {
			int map_pos = y * current_map->map_width + x;
			if (map[map_pos] == ENEMY) {
				if (x == player_pos_x && y == player_pos_y) {
					game->game_over = true;
				}
				int next_x, next_y;
				enemy_old_pos.items[enemy_old_pos.count++].value = map_pos;

				if (x < player_pos_x)
					next_x = x + 1;
				else if (x > player_pos_x)
					next_x = x - 1;
				if (y < player_pos_y)
					next_y = y + 1;
				else if (y > player_pos_y)
					next_y = y - 1;

				int pos = next_y * current_map->map_width + next_x;
				if (!hit_wall(pos, *current_map))
					array_fill(current_map, &enemy_new_pos, pos);
			}
		}
	}
	for (int i = 0; i < enemy_old_pos.count; ++i) {
		int new = enemy_old_pos.items[i].value;
		if (new < 0 || new >= current_map->map_width * current_map->map_height)
			continue;
		map[new] = SPACE;
	}
	for (int i = 0; i < enemy_new_pos.count; ++i) {
		int new = enemy_new_pos.items[i].value;
		if (new < 0 || new >= current_map->map_width * current_map->map_height)
			continue;
		if (enemy_new_pos.items[i].count < 2 && enemy_new_pos.items[i].count >= 1)
			map[new] = ENEMY;
		else if (enemy_new_pos.items[i].count >= 2) {
			map[new] = FRIEND;
		}
	}
	current_map->player.has_moved = false;
}

/*:update logic for the game
 * - check for key inputs
 */
void game_update(GameState *game) {
	mouse_control(game);
	control(game);
	update_player(game);
	enemy_update(game);
	// UpdateTexture(game->canvas, game->image.data);
}

void game_over_screen(GameState *game) {
	if (game->game_over) {
		const char *text = "You died!";
		int text_size = MeasureText(text, 200);
		DrawText(text, CANVAS_WIDTH/2 - text_size/2, CANVAS_HEIGHT/2 - 200/2, 200, BLUE);
	}
}

// :minimap
// render player moving on minimap
// static float aspect_ratio = CANVAS_WIDTH/CANVAS_HEIGHT;
static float aspect_ratio = 1.f;
static float scale_x = 1.f*10;
static float scale_y = 1.f*10;
// static float scale_x = CANVAS_WIDTH/100;
// static float scale_y = CANVAS_HEIGHT/100;

void draw_minimap(GameState *game) {
	int size_w = 10;
	int size_h = 10;
	Map map = game->maps[game->current_map_index];
	float player_x = map.player.pos.x;
	float player_y = map.player.pos.y;
	for (int y = 0; y < map.map_height; ++y) {
		for (int x = 0; x < map.map_width; ++x) {
			int mp = y * map.map_width + x;
			switch (map.map[mp]) {
				case WALL:
					DrawRectangle(x*aspect_ratio*scale_x, y*scale_y, size_w, size_h, YELLOW);
				break;
				case ENEMY:
					DrawRectangle(x*aspect_ratio*scale_x, y*scale_y, 5, 5, RED);
				break;
				case FRIEND:
					DrawRectangle(x*aspect_ratio*scale_x, y*scale_y, 5, 5, GREEN);
				break;
			}
		}
	}
	DrawRectangle(player_x*aspect_ratio*scale_x, player_y*scale_y, 5, 5, BLUE);
	Vector2 p = {player_x*aspect_ratio*scale_x + 0.5f, player_y*scale_y + 0.5f};
	Vector2 d = {map.player.dir.x*aspect_ratio*scale_x*5, map.player.dir.y*scale_y*5};
	DrawLineV(p, Vector2Add(p, d), BLUE);
}

void render(GameState *game) {
	game_update(game);
	BeginDrawing();
	ClearBackground(GetColor(BACKGROUND));
	// DrawTexture(game->canvas, 0, 0, WHITE);
	raycast(game);
	game_over_screen(game);
	DrawFPS(10, 10);
	draw_minimap(game);
	EndDrawing();
}
