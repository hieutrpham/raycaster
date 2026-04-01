#include "plug.h"
#include "raylib.h"

#ifdef HOT_RELOAD
void (*render)(GameState *game);
#else
void render(GameState *game);
#endif

Player player = {
	.pos = (Vector2){.x = 1.5f, .y = 1.5f},
	.dir = (Vector2){.x = 0, .y = 1},
	.angle = PI/2,
};

Map map1 = {
	.map = {
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,2,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	},
	.map_height = 15,
	.map_width = 16
};

Map map2 = {
	.map = {
		1,1,1,1,1,1,1,1,
		1,0,0,0,1,0,0,1,
		1,0,1,0,1,0,0,1,
		1,0,0,0,1,0,0,1,
		1,0,0,0,0,0,0,1,
		1,0,1,0,0,1,0,1,
		1,0,1,0,0,0,0,1,
		1,1,1,1,1,1,1,1,
	},
	.map_height = 8,
	.map_width = 8
};

void game_init() {
}

void game_shutdown() {
}

int main(void)
{
#ifdef HOT_RELOAD
	char *libplug = "./libplug.so";
	void *lib = dlopen(libplug, RTLD_NOW);
	if (!lib)
		return fprintf(stderr, "%s\n", dlerror());
	render = dlsym(lib, "render");
#endif
	
	InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Raycaster");
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	Image wall = LoadImage("./Bookshelf_64.png");
	Image image = GenImageColor(CANVAS_WIDTH, CANVAS_HEIGHT, BLACK); // generate new blank image
	Texture2D canvasTex = LoadTextureFromImage(image);
	Texture2D texture_wall = LoadTextureFromImage(wall);
	// Setting up the game data
	GameState game = {
		.player = player,
		.maps = {map1, map2},
		.current_map_index = 0,
		.canvas = canvasTex,
		.image = image,
		.wall = texture_wall,
	};
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
	UnloadImage(wall);
	UnloadImage(image);
	UnloadTexture(canvasTex);
	UnloadTexture(texture_wall);
	CloseWindow();
	return 0;
}
