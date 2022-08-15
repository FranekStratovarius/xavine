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

#include "components/general.h"

namespace xavine {
	class Window {
		public:
			Window();
			Window(char* window_name);
			~Window();
			int get_monitor_refresh_rate();
			bool should_close();
			void poll_events();
			void empty_draw_call();
			void render_frame(flecs::world* world);
			Input* get_input();
			int get_width();
			int get_height();
		private:
			char* window_name;
			GLFWwindow* window;
			int width, height;
			bgfx::ViewId kClearView;
			Input* input;
			int refresh_rate;
	};
}