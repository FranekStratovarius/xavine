#include <stdio.h>
#include <flecs.h>
#include "graphics_init.h"

int main(void) {
	// initialization
	flecs::world world;
	xavine::Renderer* renderer;
	try {
		renderer = new xavine::Renderer();
	} catch (const char* message) {
		fprintf(stderr, "initialization failed: %s\n", message);
		return 1;
	}

	if (renderer == nullptr) {
		fprintf(stderr, "no renderer created\n");
		return 1;
	}


	// render loop
	while (renderer->render());

	// cleanup
	delete renderer;

	return 0;
}
