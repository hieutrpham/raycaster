#include "plug.h"
#include "raylib.h"
#include <math.h>

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
	Player player = {
		.pos = (Vector2){.x = 3.0f, .y = 3.0f},
		.dir = (Vector2){.x = cos(PI/2), .y = sin(PI/2)},
		.angle = PI/2,
	};

	GameState game = {
		.p = player,
		.maps = {}, // TODO: need an array of maps
		.current_map_index = 0
	};

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
