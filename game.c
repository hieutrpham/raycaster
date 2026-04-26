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
	Player *p = &game->maps[game->current_map_index].player;
	Map current_map = game->maps[game->current_map_index];
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

		float tx = distH < distV ? fmod(hx, 1.f) : fmod(vy, 1.f);
		if (current_map.map[mp_correct] == WALL) {
			draw_texture(current_map.wall_texture, tx, (float)r, line_offset, wall_height, WHITE);
		} else if (current_map.map[mp_correct] == ENEMY) {
			draw_texture(game->enemy_texture, tx, (float)r, line_offset, wall_height, WHITE);
		} else {
			draw_texture(game->friend_texture, tx, (float)r, line_offset, wall_height, WHITE);
		}
	}
}

void draw_texture(Texture2D texture, float tx, float dest_x, float dest_y, float dest_height, Color color) {
	// float brightness = Clamp(dest_height/CANVAS_HEIGHT, 0.f, 1.f);
	float brightness = 1.f;
	Rectangle src_rec = {tx * texture.width, 0, 1, texture.height};
	Rectangle dest_rec = {dest_x, dest_y, 1, dest_height};
	Color tint = {color.r * brightness, color.g * brightness, color.b * brightness, 255};
	DrawTexturePro(texture, src_rec, dest_rec, (Vector2){0, 0}, 0.f, tint);
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
					game->screen_type = END_SCREEN;
					return;
				}
				int next_x = x;
				int next_y = y;
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
}

// TODO: add restart and quit button
void game_over_screen(GameState *game) {
	(void)game;
	ShowCursor();
	draw_text_center("You died!", 200, BLUE);
}

// :minimap
void draw_minimap(GameState *game) {
	float aspect_ratio = 1.f;
	float scale_x = 1.f*10;
	float scale_y = 1.f*10;
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
	Vector2 p = {player_x*aspect_ratio*scale_x + 2.5f, player_y*scale_y + 2.5f};
	Vector2 d = {map.player.dir.x*aspect_ratio*scale_x*5, map.player.dir.y*scale_y*5};
	DrawLineV(p, Vector2Add(p, d), BLUE);
}

/* :draw_button function
 * calculate the rectangle width, height based on the text width and font size
 * also need to pass in an origin vector
*/
void draw_button(const char *text, int fontSize, Color color_rec, Color color_text, Vector2 origin) {
	// const int padding = 5;
	int text_width = MeasureText(text, fontSize);
	Rectangle rec = {.x = origin.x, .y = origin.y, .width = text_width, .height = fontSize};
	DrawRectangleRec(rec, color_rec);
	DrawText(text, origin.x, origin.y, fontSize, color_text);
}

// :test
typedef struct {
	Rectangle rec;
	Color button_color;
	const char *name;
	int font_size;
	bool is_hovered;
} Button;

Button init_button(const char *name, int font_size, Vector2 origin, Color color) {
	Button b = {0};
	int text_width = MeasureText(name, font_size);
	b.name = name;
	b.font_size = font_size;
	b.rec = (Rectangle){.x = origin.x, .y = origin.y, .width = text_width, .height = font_size};
	b.button_color = color;
	return b;
}

void render_button(Button b) {
	DrawRectangleRec(b.rec, b.button_color);
	DrawText(b.name, b.rec.x, b.rec.y, b.font_size, WHITE);
}

void test_screen(GameState *game) {
	(void)game;
	ClearBackground(DARKPURPLE);
	Vector2 mouse_pos = GetMousePosition();
	const int padding = 50;
	Vector2 origin = {CANVAS_WIDTH/2, CANVAS_HEIGHT/2};
	Button start_button = init_button("start", 40, origin, GREEN);
	Button test_button = init_button("test", 40, (Vector2){origin.x, origin.y + padding}, GREEN);
	Button end_button = init_button("end", 40, (Vector2){origin.x, origin.y + padding*2}, GREEN);
	static int index = -1;
	Button button_array[] = {start_button, test_button, end_button};
	if (IsKeyPressed(KEY_J))
		index = (index + 1) % ARRAY_LEN(button_array);
	for (int i = 0; i < (int)ARRAY_LEN(button_array); ++i) {
		if (CheckCollisionPointRec(mouse_pos, button_array[i].rec) || index == i) {
			button_array[i].button_color = ORANGE;
		}
	}
	for (int i = 0; i < (int)ARRAY_LEN(button_array); ++i) {
		render_button(button_array[i]);
	}
}

// :button with side effect
void interactive_button (GameState *game, GameScreen screen_type, Vector2 mouse_pos, Rectangle rec, const char *str) {
	if (CheckCollisionPointRec(mouse_pos, rec)) {
		draw_button(str, 40, ORANGE, WHITE, (Vector2){.x = rec.x, .y = rec.y});
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			game->screen_type = screen_type;
	}
	else
		draw_button(str, 40, GREEN, WHITE, (Vector2){.x = rec.x, .y = rec.y});
}

void draw_text_center(const char* str, const int size, Color color) {
	int str_width = MeasureText(str, size);
	DrawText(str, CANVAS_WIDTH/2-str_width/2, CANVAS_HEIGHT/2 - size/2, size, color);
}

// TODO: add support for buttons navigation
// :start_screen
void start_screen(GameState *game) {
	ClearBackground(DARKBLUE);
	draw_text_center("Angry Cubes", 100, RED);
	Vector2 mouse_pos = GetMousePosition();
	const int padding = 50;
	const int x_origin = CANVAS_WIDTH/2;
	const int y_origin = CANVAS_HEIGHT/2 + 100;
	Rectangle start_rec = {.x = x_origin, .y = y_origin, .width = 100, .height = 40};
	interactive_button(game, GAME_SCREEN, mouse_pos, start_rec, "start");

	Rectangle test_rec = {.x = x_origin, .y = y_origin + padding, .width = 100, .height = 40};
	interactive_button(game, TEST_SCREEN, mouse_pos, test_rec, "test");

	Rectangle end_rec = {.x = x_origin, .y = y_origin + padding*2, .width = 100, .height = 40};
	interactive_button(game, QUIT_GAME, mouse_pos, end_rec, "quit");

	Rectangle rec_array[3] = {start_rec, test_rec, end_rec};
	static int index = 0;
	if (IsKeyPressed(KEY_J)) {
		index = (index + 1) % sizeof(rec_array);
	}
}

void render(GameState *game, bool *game_over) {
	BeginDrawing();
	ClearBackground(GetColor(BACKGROUND));

	switch (game->screen_type) {
		case START_SCREEN:
			ShowCursor();
			start_screen(game);
			break;
		case TEST_SCREEN:
			test_screen(game);
			break;
		case GAME_SCREEN:
			HideCursor();
			ClearBackground(DARKGREEN);
			game_update(game);
			raycast(game);
			break;
		case END_SCREEN:
			game_over_screen(game);
			break;
		case QUIT_GAME:
			*game_over = true;
	}
	#ifdef DEBUG
	DrawFPS(10, 10);
	draw_minimap(game);
	#endif
	EndDrawing();
}
