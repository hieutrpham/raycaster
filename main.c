#include "plug.h"

void (*render)(Player *player);

// TODO: implement a game state struct in main and pass it to render.
// * render is a function pointer that get hot reloaded from a shared object file
// * Game state would have the player struct
//
// NOTE: how should i port the cub3d to raylib?
// * start fresh and use the cub3d math as reference. lots more work
// * copy the structure of cub3d over. but this is highly abstracted with a lot of refactor done
// * continue with the current code base in raycaster. try to fix it and clean it up
//
int main(void)
{
	Player player = {
		.x = 500,
		.y = 500,
		.angle = PI/2,
		.dx = 1,
		.dy = 1
	};

	char *libplug = "./libplug.so";
	void *lib = dlopen(libplug, RTLD_NOW);
	if (!lib)
		return fprintf(stderr, "%s\n", dlerror());
	render = dlsym(lib, "render");
	InitWindow(1920, 1080, "Raycaster");
	SetTargetFPS(60);
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
		render(&player);
	}
	CloseWindow();
	return 0;
}
