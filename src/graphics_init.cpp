#include "graphics_init.h"

bgfx::ShaderHandle loadShader(const char *FILENAME) {
    const char* shaderPath = "???";

    switch (bgfx::getRendererType()) {
        case bgfx::RendererType::Noop:
        case bgfx::RendererType::Direct3D9:  shaderPath = "assets/shaders/dx9/";   break;
        case bgfx::RendererType::Direct3D11: shaderPath = "assets/shaders/dx11/";  break;
        case bgfx::RendererType::Direct3D12: shaderPath = "assets/shaders/dx12/";  break;
        case bgfx::RendererType::Gnm:        shaderPath = "assets/shaders/pssl/";  break;
        case bgfx::RendererType::Metal:      shaderPath = "assets/shaders/metal/"; break;
        case bgfx::RendererType::OpenGL:     shaderPath = "assets/shaders/glsl/";  break;
        case bgfx::RendererType::OpenGLES:   shaderPath = "assets/shaders/essl/";  break;
        case bgfx::RendererType::Vulkan:     shaderPath = "assets/shaders/spirv/"; break;
    }

    size_t shaderLen = strlen(shaderPath);
	printf("Loading shader: %s%s\n", shaderPath, FILENAME);
    size_t fileLen = strlen(FILENAME);
    char *filePath = (char *)malloc(shaderLen + fileLen);
    memcpy(filePath, shaderPath, shaderLen);
    memcpy(&filePath[shaderLen], FILENAME, fileLen);

    FILE *file = fopen(filePath, "rb");
	printf("Loading shader: %s\n", filePath);
    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
    fread(mem->data, 1, fileSize, file);
    mem->data[mem->size - 1] = '\0';
    fclose(file);

    return bgfx::createShader(mem);
}

static bool s_showStats = false;

static void glfw_errorCallback(int error, const char *description) {
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

static void glfw_keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_F1 && action == GLFW_RELEASE) {
		s_showStats = !s_showStats;
	}
}


xavine::Renderer::Renderer() {
	// Create a GLFW window without an OpenGL context.
	glfwSetErrorCallback(glfw_errorCallback);
	if (!glfwInit()) {
		throw "glfw initialization failed";
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "bgfx", nullptr, nullptr);
	if (!window) {
		throw "could not create glfw window";
	}
	glfwSetKeyCallback(window, glfw_keyCallback);
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
	glfwGetWindowSize(window, &width, &height);
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = BGFX_RESET_VSYNC;
	if (!bgfx::init(init)) {
		throw "bgfx initialization failed";
	}
	// Set view 0 to the same dimensions as the window and to clear the color buffer.
	//bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR);
	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x443355FF, 1.0f, 0);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

	bgfx::VertexLayout pcvDecl;
    pcvDecl.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
    .end();
    vbh = bgfx::createVertexBuffer(bgfx::makeRef(cubeVertices, sizeof(cubeVertices)), pcvDecl);
    ibh = bgfx::createIndexBuffer(bgfx::makeRef(cubeTriList, sizeof(cubeTriList)));

	vsh = loadShader("vs_base.bin");
    fsh = loadShader("fs_base.bin");
    program = bgfx::createProgram(vsh, fsh, true);

	counter = 0;
}

bool xavine::Renderer::render() {
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

	const bx::Vec3 at = {0.0f, 0.0f,  0.0f};
	const bx::Vec3 eye = {0.0f, 0.0f, -5.0f};
	float view[16];
	bx::mtxLookAt(view, eye, at);
	float proj[16];
	bx::mtxProj(proj, 60.0f, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
	bgfx::setViewTransform(0, view, proj);
	float mtx[16];
	bx::mtxRotateXY(mtx, counter * 0.01f, counter * 0.01f);
	bgfx::setTransform(mtx); 

	bgfx::setVertexBuffer(0, vbh);
	bgfx::setIndexBuffer(ibh);

	bgfx::submit(0, program);

	// Use debug font to print information about this example.
	bgfx::dbgTextClear();
	bgfx::dbgTextPrintf(0, 0, 0x0f, "Press F1 to toggle stats.");
	bgfx::dbgTextPrintf(0, 1, 0x0f, "Color can be changed with ANSI \x1b[9;me\x1b[10;ms\x1b[11;mc\x1b[12;ma\x1b[13;mp\x1b[14;me\x1b[0m code too.");
	bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
	bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
	const bgfx::Stats* stats = bgfx::getStats();
	bgfx::dbgTextPrintf(0, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);
	// Enable stats or debug text.
	bgfx::setDebug(s_showStats ? BGFX_DEBUG_STATS : BGFX_DEBUG_TEXT);
	// Advance to next frame. Process submitted rendering primitives.
	bgfx::frame();
	counter++;

	return !glfwWindowShouldClose(window);
}

xavine::Renderer::~Renderer() {
	bgfx::shutdown();
	glfwTerminate();
}