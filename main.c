#include "game.h"
#include "raylib.h"

#ifdef HOT_RELOAD
void (*render)(GameState *game);
#else
void render(GameState *game);
#endif

Image images[IMAGE_COUNT];
Texture2D textures[IMAGE_COUNT];

Map maps[MAP_COUNT] = {
#define P (0xff) // denote player on the map
{
#if 1
		.map = {
			1,1,1,1,1,1,1,1,
			1,0,0,0,0,0,2,1,
			1,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,1,
			1,0,0,0,0,0,2,1,
			1,1,1,1,1,1,1,1,
		},
		.map_height = 8,
		.map_width = 8,
		.player = {
			.pos = (Vector2){.x = 1.5f, .y = 1.5f},
			.dir = (Vector2){.x = 0, .y = 1},
			.angle = PI/2,
		},
	#else
		.map = {
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			1,0,0,0,0,0,0,0,0,0,2,0,0,0,0,1,
			1,0,0,0,0,0,2,0,2,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,2,0,0,1,
			1,0,0,0,2,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,P,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,2,0,0,0,0,0,0,0,2,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,1,
			1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,1,
			1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		},
		.map_height = 15,
		.map_width = 16,
		.player = {
			.pos = (Vector2){.x = 8.5f, .y = 7.5f},
			.dir = (Vector2){.x = 0, .y = 1},
			.angle = PI/2,
		},
#endif
	},

	{
		.map = {
			1,1,1,1,1,1,1,1,
			1,0,0,0,1,0,0,1,
			1,0,1,0,1,0,0,1,
			1,0,0,P,1,0,0,1,
			1,0,0,0,0,0,0,1,
			1,0,1,0,0,1,0,1,
			1,0,1,0,0,0,0,1,
			1,1,1,1,1,1,1,1,
		},
		.map_height = 8,
		.map_width = 8,
		.player = {
			.pos = (Vector2){.x = 3.5f, .y = 3.5f},
			.dir = (Vector2){.x = 0, .y = 1},
			.angle = PI/2,
		},
	}
#undef P
};

// Image image; // generate new blank image
// Texture2D canvasTex;
GameState game;

void game_init() {
#ifdef HOT_RELOAD
	char *libplug = "./libplug.so";
	void *lib = dlopen(libplug, RTLD_NOW);
	if (!lib)
		return fprintf(stderr, "%s\n", dlerror());
	render = dlsym(lib, "render");
#endif

	images[MAP_0_TEXTURE] = LoadImage("./assets/Bookshelf_64.png");
	textures[MAP_0_TEXTURE] = LoadTextureFromImage(images[MAP_0_TEXTURE]);

	for (int i = 0; i < MAP_COUNT; ++i) {
		maps[i].wall_texture = textures[i];
	}

	// image = GenImageColor(CANVAS_WIDTH, CANVAS_HEIGHT, BLACK); // generate new blank image
	// canvasTex = LoadTextureFromImage(image);

	for (int i = 0; i < MAP_COUNT; ++i) {
		game.maps[i] = maps[i];
	}
	game.current_map_index = 0;
}

void game_shutdown() {
	for (int i = 0; i < IMAGE_COUNT; ++i) {
		UnloadImage(images[i]);
		UnloadTexture(textures[i]);
	}

	// UnloadImage(image);
	// UnloadTexture(canvasTex);
}

int main(void)
{
	InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Raycaster");
	game_init();
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	HideCursor();

	while (!WindowShouldClose()) {
		#ifdef HOT_RELOAD
		if (IsKeyPressed(KEY_R))
		{
			dlclose(lib);
			lib = dlopen(libplug, RTLD_NOW);
			if (!lib)
				return fprintf(stderr, "%s\n", dlerror());
			render = dlsym(lib, "render");
			if (!render)
				fprintf(stderr, "%s\n", dlerror());
		}
		#endif
		render(&game);
	}
	game_shutdown();
	CloseWindow();
	return 0;
}
