#include <stdio.h>
#include <iostream>
#include <thread>

#include <flecs.h>
#include <flecs_os_api_stdcpp.h>

#include <bx/bx.h>
#include <bx/math.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3.h>

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#	if ENTRY_CONFIG_USE_WAYLAND
#		include <wayland-egl.h>
#		define GLFW_EXPOSE_NATIVE_WAYLAND
#	else
#		define GLFW_EXPOSE_NATIVE_X11
#		define GLFW_EXPOSE_NATIVE_GLX
#	endif
#elif BX_PLATFORM_OSX
#	define GLFW_EXPOSE_NATIVE_COCOA
#	define GLFW_EXPOSE_NATIVE_NSGL
#elif BX_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define GLFW_EXPOSE_NATIVE_WGL
#endif //
#include <GLFW/glfw3native.h>
#include "graphics.h"

// Component types
struct Position {
    double x;
    double y;
	double z;
};

struct Render_Data {
	bgfx::VertexBufferHandle vertex_buffer_handle;
	bgfx::IndexBufferHandle index_buffer_handle;
	bgfx::ProgramHandle program;
};

struct Time_Data {
	float delta_time;
};

// cube datas
struct PosColorVertex {
	float x;
	float y;
	float z;
	uint32_t abgr;
};

// input datas
struct KeyState {
	bool pressed = false;
	bool released = false;
	bool state = false;
};

struct MouseCoord {
	float x = 0;
	float y = 0;
};

struct MouseState {
	KeyState mouse_key_state[8];	// glfw can get 8 mouse buttons
	MouseCoord position;
};

struct Input {
	MouseState mouse_state;
	KeyState key_state[128];	// some keyboards can have up to 110 keys
};

static PosColorVertex cubeVertices[] = {
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t cubeTriList[] = {
	0, 1, 2,
	1, 3, 2,
	4, 6, 5,
	5, 6, 7,
	0, 2, 4,
	4, 2, 6,
	1, 5, 3,
	5, 7, 3,
	0, 4, 1,
	4, 5, 1,
	2, 3, 6,
	6, 3, 7,
};

void glfw_errorCallback(int error, const char* description) {
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Input* input = (Input*)glfwGetWindowUserPointer(window);

	if (action == GLFW_PRESS) {
		input->key_state[scancode].state = true;
	} else if (action == GLFW_RELEASE) {
		input->key_state[scancode].state = false;
	}

	printf("key: %d, state: %d\n", scancode, input->key_state[scancode].state);
}

void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	float sensitivity = 0.001f;
	Input* input = (Input*)glfwGetWindowUserPointer(window);
	input->mouse_state.position.x = (float)xpos * sensitivity;
	input->mouse_state.position.y = (float)ypos * sensitivity;
}

int main(void) {
	// Create a GLFW window without an OpenGL context.
	glfwSetErrorCallback(glfw_errorCallback);
	if (!glfwInit()) {
		throw "glfw initialization failed";
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
 
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "xavine", monitor, nullptr);
	if (!window) {
		throw "could not create glfw window";
	}
	// Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
	// Most graphics APIs must be used on the same thread that created the window.
	bgfx::renderFrame();
	// Initialize bgfx using the native window handle and window resolution.
	bgfx::Init init;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
# 		if ENTRY_CONFIG_USE_WAYLAND
		wl_egl_window *win_impl = (wl_egl_window*)glfwGetWindowUserPointer(window);
		if(!win_impl) {
			int width, height;
			glfwGetWindowSize(window, &width, &height);
			struct wl_surface* surface = (struct wl_surface*)glfwGetWaylandWindow(window);
			if(!surface)
				return nullptr;
			win_impl = wl_egl_window_create(surface, width, height);
			glfwSetWindowUserPointer(window, (void*)(uintptr_t)win_impl);
		}
		init.platformData.nwh = (void*)(uintptr_t)win_impl;
		init.platformData.ndt = glfwGetWaylandDisplay();
#		else
		init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
		init.platformData.ndt = glfwGetX11Display();
#		endif
#elif BX_PLATFORM_OSX
	init.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
	init.platformData.nwh = glfwGetWin32Window(window);
#endif
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init)) {
		throw "bgfx initialization failed";
	}
	// Set view 0 to the same dimensions as the window and to clear the color buffer.
	const bgfx::ViewId kClearView = 0;
	//bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

	// initialization
	stdcpp_set_os_api();
	flecs::world world;
	Input input_local;
	Input* input = &input_local;
	glfwSetWindowUserPointer(window, input);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_FALSE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(window, glfw_key_callback);
	glfwSetCursorPosCallback(window, glfw_cursor_position_callback);
	world.set_target_fps(mode->refreshRate);
	unsigned int max_thread_count = std::thread::hardware_concurrency();
	ecs_set_threads(world, max_thread_count);
	printf("threads: %d\n", world.get_threads());

	// create time update system
	flecs::entity time_sys = world.system<Time_Data>()
	.iter([](flecs::iter& it, Time_Data* td) {
		for (size_t i : it) {
			td[i].delta_time += it.delta_time() * 150.0f;
		}
		//printf("fps: %f\n", 1.0/it.delta_time());
	});

	// add time update system to pipeline
	time_sys.add(flecs::PreUpdate);

	// create render system
	flecs::entity render_sys = world.system<Position, Render_Data, Time_Data>()
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

	// create move system
	flecs::entity move_sys = world.system<Position>()
	.iter([input](flecs::iter& it, Position* pos){
		// w = 25
		const int scancode_w = glfwGetKeyScancode(GLFW_KEY_W);
		// a = 38
		const int scancode_a = glfwGetKeyScancode(GLFW_KEY_A);
		// s = 39
		const int scancode_s = glfwGetKeyScancode(GLFW_KEY_S);
		// d = 40
		const int scancode_d = glfwGetKeyScancode(GLFW_KEY_D);
		for (size_t i : it) {
			float speed = 10 * it.delta_time();

			float dir_x = 0;
			if (input->key_state[scancode_d].state) {	// d pressed
				dir_x += speed;
			} else if (input->key_state[scancode_a].state) {	// a pressed
				dir_x -= speed;
			}

			float dir_z = 0;
			if (input->key_state[scancode_w].state) {	// w pressed
				dir_z += speed;
			} else if (input->key_state[scancode_s].state) {	// s pressed
				dir_z -= speed;
			}

			pos[i].x += dir_x;
			pos[i].z += dir_z;
		}
	});

	// add move system to pipeline
	move_sys.add(flecs::OnUpdate);

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
		testcubes[i] = world.entity()
			.set<Position>({0, 0, 0})
			.set<Render_Data>({vertex_buffer_handle, index_buffer_handle, program})
			.set<Time_Data>({0.0f});
	}

	for (size_t i = 0; i < side_length; i++) {
		for (size_t j = 0; j < depth_length; j++) {
			testcubes[i * depth_length + j].set<Position>({(i - side_length / 2.0) * 4, 0, (double)j * 4});
		}
	}

	// show all entities
	/*
	world.each([](flecs::entity e, Position& p) {
		std::cout << e.name() << ": {" << p.x << ", " << p.y << ", " << p.z << "}" << std::endl;
	});
	*/
	
	// run game loop	
	while (!glfwWindowShouldClose(window)) {
		// poll input events
		glfwPollEvents();
		// Handle window resize.
		int oldWidth = width, oldHeight = height;
		glfwGetWindowSize(window, &width, &height);
		if (width != oldWidth || height != oldHeight) {
			bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
			bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
		}
		// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
		bgfx::touch(kClearView);

		// run the ecs world
		world.progress();

		// render
		// set the view transform matrix
		//const bx::Vec3 at = {0.0f, 10.0f,  30.0f};
		//const bx::Vec3 eye = {0.0f, 50.0f, -30.0f};
		const bx::Vec3 at = {0.0f, 50.0f,  30.0f};
		const bx::Vec3 eye = {0.0f, 50.0f, -30.0f};
		float mtx_rotate[16];
		bx::mtxIdentity(mtx_rotate);
		bx::mtxRotateZYX(mtx_rotate, input->mouse_state.position.y, input->mouse_state.position.x, 0.0f);
		float view[16];
		bx::mtxLookAt(view, eye, at);
		float mtx_result[16];
		bx::mtxMul(mtx_result, view, mtx_rotate);
		float proj[16];
		bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 2000.0f, bgfx::getCaps()->homogeneousDepth);
		bgfx::setViewTransform(0, mtx_result, proj);
		// Enable stats or debug text.
		bgfx::setDebug(BGFX_DEBUG_STATS);
		// Advance to next frame. Process submitted rendering primitives.
		bgfx::frame();
	}

	// cleanup
	bgfx::shutdown();
	glfwTerminate();

	return 0;
}
