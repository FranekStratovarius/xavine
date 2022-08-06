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

namespace xavine {
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

	class Window {
		public:
			Window();
			Window(char* window_name);
			~Window();
			int get_monitor_refresh_rate();
			bool should_close();
			void poll_events();
			void empty_draw_call();
			void render_frame();
		private:
			char* window_name;
			GLFWwindow* window;
			int width, height;
			bgfx::ViewId kClearView;
			Input* input;
			int refresh_rate;
	};
}