#include "game.h"
#include <assert.h>
#include <stdio.h>

Map maps[] = {
	{
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
		.map_width = 8,
		.player = {
			.pos = (Vector2){.x = 3.5f, .y = 3.5f},
			.dir = (Vector2){.x = 0, .y = 1},
			.angle = PI/2,
		},
	},
};

void print_array(const StaticArray array) {
	for (int i = 0; i < array.count; ++i) {
		printf("value: %d, count: %d\n", array.items[i].value, array.items[i].count);
	}
}

void test_array_fill() {
	StaticArray test_array = {0};
	array_fill(maps, &test_array, 12);
	array_fill(maps, &test_array, 12);
	array_fill(maps, &test_array, 12);
	array_fill(maps, &test_array, 14);
	array_fill(maps, &test_array, 14);
	array_fill(maps, &test_array, 15);
	assert(test_array.count == 3);
	assert(test_array.items[0].count == 3);
	assert(test_array.items[0].value == 12);
	// print_array(test_array);
}

int main() {
	test_array_fill();
	printf("All tests passed!\n");
	return 0;
}
