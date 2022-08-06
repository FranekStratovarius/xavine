#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include "graphics.h"

#include "loader/cube_loader.h"
#include "components/cube.h"

void load_cubes(flecs::world* world) {
	// load mesh and shader
	bgfx::VertexLayout pcvDecl;
	pcvDecl.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
	.end();

	bgfx::VertexBufferHandle vertex_buffer_handle = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
	bgfx::IndexBufferHandle index_buffer_handle = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));
	
	bgfx::ShaderHandle vertex_shader_handle = loadShader("vs_base.bin");
	bgfx::ShaderHandle fragment_shader_handle = loadShader("fs_base.bin");
    bgfx::ProgramHandle program = bgfx::createProgram(vertex_shader_handle, fragment_shader_handle, true);

	// create entities
	const size_t side_length = 50;
	const size_t depth_length = 50;
	const size_t n_testcubes = side_length*depth_length;
	flecs::entity testcubes[n_testcubes];
	for (size_t i = 0; i < n_testcubes; i++) {
		testcubes[i] = world->entity()
			.set<Position>({0, 0, 0})
			.set<Render_Data>({vertex_buffer_handle, index_buffer_handle, program})
			.set<Time_Data>({0.0f});
	}

	for (size_t i = 0; i < side_length; i++) {
		for (size_t j = 0; j < depth_length; j++) {
			testcubes[i * depth_length + j].set<Position>({(i - side_length / 2.0) * 4, 0, (double)j * 4});
		}
	}
}