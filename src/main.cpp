#include <cstdio>
#include <iostream>
#include <thread>

#include "components/general.h"
#include "components/player.h"
#include "systems/cubes.h"
#include "systems/player.h"
#include "loader/cube_loader.h"

#include "xavine-window.h"

int main(void) {
	// initialization
	// set threading api for flecs
	stdcpp_set_os_api();
	// create flecs world
	flecs::world world;
	// set settings for flecs world
	unsigned int max_thread_count = std::thread::hardware_concurrency();
	//ecs_set_threads(world, max_thread_count);
	//printf("threads: %d\n", world.get_threads());

	// fill game world
	xavine::add_cube_systems(&world);
	xavine::load_cubes(&world);

	xavine::add_player_systems(&world);
	flecs::entity player = world.entity()
		.add<xavine::Player>()
		.set<xavine::Position>({{0, 0, 0}})
		.set<xavine::Rotation>({});

	// set input singleton
	world.set<xavine::Input*>(xavine::window.get_input());
	
	// run game loop	
	while (!xavine::window.should_close()) {
		// get events like input or window resize
		xavine::window.poll_events();
		// make empty drawcall to have more than 0 calls if none other are submitted
		xavine::window.empty_draw_call();
		// run the ecs world
		world.progress();
		// render frame
		xavine::window.render_frame(&world);
	}

	return 0;
}
