#include <cstdio>
#include "window.h"

void glfw_errorCallback(int error, const char* description) {
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// abort when scancode outside of range
	if (scancode < 0 || scancode >= 128) {
		return;
	}
	xavine::Input* input = (xavine::Input*)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS) {
		input->key_state[scancode].state = true;
	} else if (action == GLFW_RELEASE) {
		input->key_state[scancode].state = false;
	}
	printf("key: %d, state: %d\n", scancode, input->key_state[scancode].state);
}

void glfw_cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	float sensitivity = 0.001f;
	xavine::Input* input = (xavine::Input*)glfwGetWindowUserPointer(window);
	input->mouse_state.position.x = (float)xpos * sensitivity;
	input->mouse_state.position.y = (float)ypos * sensitivity;
}

xavine::Window::Window() {
	// Create a GLFW window without an OpenGL context.
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

	this->refresh_rate = mode->refreshRate;
	
	this->window = glfwCreateWindow(mode->width, mode->height, "xavine", monitor, nullptr);
	if (!this->window) {
		throw "could not create glfw window";
	}

	// Initialize bgfx using the native window handle and window resolution.
	bgfx::Init init;
#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
# 		if ENTRY_CONFIG_USE_WAYLAND
			wl_egl_window *win_impl = (wl_egl_window*)glfwGetWindowUserPointer(this->window);
			if(!win_impl) {
				glfwGetWindowSize(this->window, &this->width, &this->height);
				struct wl_surface* surface = (struct wl_surface*)glfwGetWaylandWindow(this->window);
				if(!surface)
					return nullptr;
				win_impl = wl_egl_window_create(surface, this->width, this->height);
				glfwSetWindowUserPointer(this->window, (void*)(uintptr_t)win_impl);
			}
			init.platformData.nwh = (void*)(uintptr_t)win_impl;
			init.platformData.ndt = glfwGetWaylandDisplay();
#		else
			init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(this->window);
			init.platformData.ndt = glfwGetX11Display();
#		endif
#	elif BX_PLATFORM_OSX
		init.platformData.nwh = glfwGetCocoaWindow(this->window);
#	elif BX_PLATFORM_WINDOWS
		init.platformData.nwh = glfwGetWin32Window(this->window);
#	endif
	glfwGetWindowSize(this->window, &width, &height);
	init.resolution.width = (uint32_t)this->width;
	init.resolution.height = (uint32_t)this->height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init)) {
		throw "bgfx initialization failed";
	}
	// Set view 0 to the same dimensions as the window and to clear the color buffer.
	this->kClearView = 0;
	//bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(this->kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
	// Enable stats or debug text.
	bgfx::setDebug(BGFX_DEBUG_STATS);

	this->input = new struct xavine::Input;
	glfwSetWindowUserPointer(this->window, this->input);
	glfwSetInputMode(this->window, GLFW_STICKY_KEYS, GLFW_FALSE);
	glfwSetInputMode(this->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetKeyCallback(this->window, glfw_key_callback);
	glfwSetCursorPosCallback(this->window, glfw_cursor_position_callback);
}

xavine::Window::~Window() {
	// cleanup
	bgfx::shutdown();
	glfwTerminate();
}

int xavine::Window::get_monitor_refresh_rate() {
	return this->refresh_rate;
}

bool xavine::Window::should_close() {
	return glfwWindowShouldClose(this->window);
}

void xavine::Window::poll_events() {
	// poll input events
	glfwPollEvents();
	// Handle window resize.
	int oldWidth = this->width, oldHeight = this->height;
	glfwGetWindowSize(this->window, &this->width, &this->height);
	if (this->width != oldWidth || this->height != oldHeight) {
		bgfx::reset((uint32_t)this->width, (uint32_t)this->height, BGFX_RESET_VSYNC);
		bgfx::setViewRect(this->kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
	}
}

void xavine::Window::empty_draw_call() {
	// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
	bgfx::touch(kClearView);
}

void xavine::Window::render_frame() {
	// set the view transform matrix
	//const bx::Vec3 at = {0.0f, 10.0f,  30.0f};
	//const bx::Vec3 eye = {0.0f, 50.0f, -30.0f};
	const bx::Vec3 at = {0.0f, 50.0f,  30.0f};
	const bx::Vec3 eye = {0.0f, 50.0f, -30.0f};
	float mtx_rotate[16];
	bx::mtxIdentity(mtx_rotate);
	bx::mtxRotateZYX(mtx_rotate, this->input->mouse_state.position.y, this->input->mouse_state.position.x, 0.0f);
	float view[16];
	bx::mtxLookAt(view, eye, at);
	float mtx_result[16];
	bx::mtxMul(mtx_result, view, mtx_rotate);
	float proj[16];
	bx::mtxProj(proj, 60.0f, float(width) / float(height), 0.1f, 2000.0f, bgfx::getCaps()->homogeneousDepth);
	bgfx::setViewTransform(0, mtx_result, proj);
	// Advance to next frame. Process submitted rendering primitives.
	bgfx::frame();
}