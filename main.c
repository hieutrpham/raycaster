#include "plug.h"
#include "raylib.h"

void (*render)(GameState *game);

int main(void)
{
	char *libplug = "./libplug.so";
	void *lib = dlopen(libplug, RTLD_NOW);
	if (!lib)
		return fprintf(stderr, "%s\n", dlerror());
	render = dlsym(lib, "render");
	
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
		render(&game);
	}
	UnloadImage(wall);
	UnloadImage(image);
	UnloadTexture(canvasTex);
	UnloadTexture(texture_wall);
	CloseWindow();
	return 0;
}
