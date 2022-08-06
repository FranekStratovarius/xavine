#include <cstdio>
#include <iostream>
#include <thread>

#include "components/cube.h"
#include "systems/cubes.h"
#include "systems/graphics.h"
#include "systems/player.h"
#include "loader/cube_loader.h"

#include "window.h"

int main(void) {
	// initialization
	// create window and renderer
	xavine::Window window = xavine::Window();
	// set threading api for flecs
	stdcpp_set_os_api();
	// create flecs world
	flecs::world world;
	// set settings for flecs world
	world.set_target_fps(window.get_monitor_refresh_rate());
	unsigned int max_thread_count = std::thread::hardware_concurrency();
	ecs_set_threads(world, max_thread_count);
	printf("threads: %d\n", world.get_threads());

	// fill game world
	add_cube_systems(&world);
	load_cubes(&world);
	
	// run game loop	
	while (!window.should_close()) {
		// get events like input or window resize
		window.poll_events();
		// make empty drawcall to have more than 0 calls if none other are submitted
		window.empty_draw_call();
		// run the ecs world
		world.progress();
		// render frame
		window.render_frame();
	}

	return 0;
}