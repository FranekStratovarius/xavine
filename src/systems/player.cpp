#include <flecs.h>
#include <GLFW/glfw3.h>
#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>

#include "systems/player.h"
#include "components/general.h"
#include "components/player.h"

void xavine::add_player_systems(flecs::world* world) {
	// create movement and looking system
	flecs::entity motion_turning_sys = world->system<xavine::Player, xavine::Position, xavine::Rotation>()
	//.each([world](flecs::entity e, xavine::Player& player, xavine::Position& position, xavine::Look_At& look_at) {
	.iter([world](flecs::iter it, xavine::Player* player, xavine::Position* pos, xavine::Rotation* rotation) {
		
		xavine::Input** pinput = world->get_mut<xavine::Input*>();
		xavine::Input* input = *pinput;
		
		// w = 25
		// a = 38
		// s = 39
		// d = 40
		const int scancode_w = glfwGetKeyScancode(GLFW_KEY_W);
		const int scancode_a = glfwGetKeyScancode(GLFW_KEY_A);
		const int scancode_s = glfwGetKeyScancode(GLFW_KEY_S);
		const int scancode_d = glfwGetKeyScancode(GLFW_KEY_D);
		const int scancode_q = glfwGetKeyScancode(GLFW_KEY_Q);
		const int scancode_e = glfwGetKeyScancode(GLFW_KEY_E);
		
		float speed = 10 * it.delta_time();

		float dir_x = 0;
		if (input->key_state[scancode_d].state) {	// d pressed
			dir_x += speed;
		} else if (input->key_state[scancode_a].state) {	// a pressed
			dir_x -= speed;
		}

		float dir_y = 0;
		if (input->key_state[scancode_w].state) {	// w pressed
			dir_y += speed;
		} else if (input->key_state[scancode_s].state) {	// s pressed
			dir_y -= speed;
		}

		float dir_z = 0;
		if (input->key_state[scancode_e].state) {	// e pressed
			dir_z += speed;
		} else if (input->key_state[scancode_q].state) {	// q pressed
			dir_z -= speed;
		}

		float rotate_x = input->mouse_state.relative.x;
		float rotate_y = input->mouse_state.relative.y;

		for (int i : it) {
			// rotate player camera with clamp and modulo constraints
			rotation[i].yaw = bx::mod(rotation[i].yaw + rotate_x, bx::kPi2);
			rotation[i].pitch = bx::clamp(rotation[i].pitch + rotate_y, -bx::kPiHalf+0.001f, bx::kPiHalf-0.001f);

			// create vector in direction of camera
			float mtx_rotationZ[16];
			float mtx_rotationZ_right[16];
			float mtx_rotationX[16];
			float mtx_rotationX_up[16];
			// rotate around z axis with x_mouse
			bx::mtxRotateZ(mtx_rotationZ, -rotation[i].yaw);
			bx::mtxRotateZ(mtx_rotationZ_right, bx::kPiHalf);
			//rotate around x axis with y_mouse
			bx::mtxRotateX(mtx_rotationX, -rotation[i].pitch);
			bx::mtxRotateX(mtx_rotationX_up, bx::kPiHalf);
			bx::Vec3 look_at = bx::Vec3{0.0f, 1.0f, 0.0f};
			bx::Vec3 look_at_right = bx::Vec3{0.0f, 1.0f, 0.0f};
			bx::Vec3 look_at_up = bx::Vec3{0.0f, 1.0f, 0.0f};
			look_at = bx::mul(look_at, mtx_rotationX);
			look_at = bx::mul(look_at, mtx_rotationZ);
			look_at = bx::mul(look_at, -dir_y);
			look_at_right = bx::mul(look_at_right, mtx_rotationZ_right);
			look_at_right = bx::mul(look_at_right, mtx_rotationX);
			look_at_right = bx::mul(look_at_right, mtx_rotationZ);
			look_at_right = bx::mul(look_at_right, dir_x);
			look_at_up = bx::mul(look_at_up, mtx_rotationX_up);
			look_at_up = bx::mul(look_at_up, mtx_rotationX);
			look_at_up = bx::mul(look_at_up, mtx_rotationZ);
			look_at_up = bx::mul(look_at_up, -dir_z);

			pos[i].position_vec = bx::add(pos[i].position_vec, look_at);
			pos[i].position_vec = bx::add(pos[i].position_vec, look_at_right);
			pos[i].position_vec = bx::add(pos[i].position_vec, look_at_up);

			//printf("x: %4f, y: %4f, z: %4f\n", pos[i].position_vec.x, pos[i].position_vec.y, pos[i].position_vec.z);
		}
	});

	motion_turning_sys.add(flecs::PostUpdate);

	// create camera drawing system
	/*
	flecs::entity render_sys = world->system<xavine::Player, xavine::Position, xavine::Look_At>()
	//.each([](flecs::entity e, xavine::Position& p, xavine::Render_Data& rd, xavine::Time_Data& td) {
	.iter([world](flecs::iter it, xavine::Player* player, xavine::Position* pos, xavine::Look_At* look_at) {
		for (int i : it) {
			const bx::Vec3 at = {look_at[i].x, look_at[i].y, look_at[i].z};
			const bx::Vec3 eye = {pos[i].x, pos[i].y, pos[i].z};
			//float mtx_rotate[16];
			//bx::mtxIdentity(mtx_rotate);
			//bx::mtxRotateZYX(mtx_rotate, this->input->mouse_state.position.y, this->input->mouse_state.position.x, 0.0f);
			float view[16];
			bx::mtxLookAt(view, eye, at);
			//float mtx_result[16];
			//bx::mtxMul(mtx_result, view, mtx_rotate);
			float proj[16];
			bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 2000.0f, bgfx::getCaps()->homogeneousDepth);
			bgfx::setViewTransform(0, view, proj);
			// Advance to next frame. Process submitted rendering primitives.
			bgfx::frame();
		}
	});
	*/
}