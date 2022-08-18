#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>

#include "systems/cubes.h"
#include "components/general.h"
#include "components/cubes.h"

void xavine::add_cube_systems(flecs::world* world) {
	// create time update system
	flecs::entity time_sys = world->system<xavine::Time_Data>()
	.iter([](flecs::iter& it, xavine::Time_Data* td) {
		for (size_t i : it) {
			td[i].delta_time += it.delta_time() * 150.0f;
		}
		//printf("fps: %f\n", 1.0/it.delta_time());
	});

	// add time update system to pipeline
	time_sys.add(flecs::PreUpdate);

	// create render system
	flecs::entity render_sys = world->system<xavine::Position, xavine::Render_Data, xavine::Time_Data>()
	.iter([](flecs::iter& it, xavine::Position* p, xavine::Render_Data* rd, xavine::Time_Data* td) {
		bgfx::Encoder* encoder = bgfx::begin();
		if (encoder == nullptr) {return;}
		for (size_t i : it) {
			float mtx_rotate[16];
			bx::mtxIdentity(mtx_rotate);
			
			bx::mtxRotateXYZ(mtx_rotate, td[i].delta_time * 0.01f, td[i].delta_time * 0.01f, 0.0f);
			float mtx_translate[16];
			bx::mtxIdentity(mtx_translate);
			bx::mtxTranslate(mtx_translate, p[i].position_vec.x, p[i].position_vec.y, p[i].position_vec.z);

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