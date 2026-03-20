#include "plug.h"

void (*render)(GameState *game);

int main(void)
{
	char *libplug = "./libplug.so";
	void *lib = dlopen(libplug, RTLD_NOW);
	if (!lib)
		return fprintf(stderr, "%s\n", dlerror());
	render = dlsym(lib, "render");
	InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Raycaster");

	// Setting up the game data
	GameState game = {
		.player = player,
		.maps = {map1, map2},
		.current_map_index = 0
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
	CloseWindow();
	return 0;
}
