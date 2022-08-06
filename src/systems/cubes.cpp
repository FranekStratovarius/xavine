#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>

#include "systems/cubes.h"
#include "components/cube.h"

void add_cube_systems(flecs::world* world) {
	// create time update system
	flecs::entity time_sys = world->system<Time_Data>()
	.iter([](flecs::iter& it, Time_Data* td) {
		for (size_t i : it) {
			td[i].delta_time += it.delta_time() * 150.0f;
		}
		//printf("fps: %f\n", 1.0/it.delta_time());
	});

	// add time update system to pipeline
	time_sys.add(flecs::PreUpdate);

		// create render system
	flecs::entity render_sys = world->system<Position, Render_Data, Time_Data>()
	//.each([](flecs::entity e, Position& p, Render_Data& rd, Time_Data& td) {
	.iter([](flecs::iter& it, Position* p, Render_Data* rd, Time_Data* td) {
		bgfx::Encoder* encoder = bgfx::begin();
		if (encoder == nullptr) {return;}
		for (size_t i : it) {
			float mtx_rotate[16];
			bx::mtxIdentity(mtx_rotate);
			
			bx::mtxRotateXYZ(mtx_rotate, td[i].delta_time * 0.01f, td[i].delta_time * 0.01f, 0.0f);
			float mtx_translate[16];
			bx::mtxIdentity(mtx_translate);
			bx::mtxTranslate(mtx_translate, p[i].x, p[i].y, p[i].z);

			float mtx_result[16];
			bx::mtxMul(mtx_result, mtx_rotate, mtx_translate);

			encoder->setTransform(mtx_result); 

			encoder->setVertexBuffer(0, rd[i].vertex_buffer_handle);
			encoder->setIndexBuffer(rd[i].index_buffer_handle);

			encoder->submit(0, rd[i].program);
		}
		bgfx::end(encoder);
	});

	// add render system to pipeline
	render_sys.add(flecs::OnUpdate);
}