#include "../game.h"
#include <assert.h>
#include <stdio.h>

static void array_fill(StaticArray *array, int value) {
	if (array->count >= MAX_ARRAY_COUNT)
		return;
	for (int i = 0; i < array->count; ++i) {
		if (array->items[i].value == value) {
			array->items[i].count++;
			return;
		}
	}
	array->items[array->count++] = (MemberInt){.value = value, .count = 1};
}

void print_array(const StaticArray array) {
	for (int i = 0; i < array.count; ++i) {
		printf("value: %d, count: %d\n", array.items[i].value, array.items[i].count);
	}
}

int main() {
	StaticArray test_array = {0};
	array_fill(&test_array, 12);
	array_fill(&test_array, 12);
	array_fill(&test_array, 12);
	array_fill(&test_array, 14);
	array_fill(&test_array, 14);
	array_fill(&test_array, 15);
	assert(test_array.count == 3);
	assert(test_array.items[0].count == 3);
	assert(test_array.items[0].value == 12);
	printf("All tests passed!\n");
	// print_array(test_array);
}
