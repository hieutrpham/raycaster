#include "game.h"

void array_fill(Map *map, StaticArray *array, int value) {
	if (array->count >= MAX_ARRAY_COUNT)
		return;
	if (value < 0 || value > map->map_width * map->map_height)
		return;
	for (int i = 0; i < array->count; ++i) {
		if (array->items[i].value == value) {
			array->items[i].count++;
			return;
		}
	}
	array->items[array->count++] = (MemberInt){.value = value, .count = 1};
}

