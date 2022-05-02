#include "glad/gl.h"
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
//#include "scripting/lua.hpp"
#include "shader.hpp"
#include "sprite.hpp"

class Window{
	public:
		Window(bool fullscreen);
		~Window();
		void render();
		void render(Sprite** sprites, unsigned int sprite_count);
		bool closed();
		void poll_events();
		void my_key_callback(const char* key,int action);
		unsigned int width, height;
	private:
		GLFWwindow* window;
		//lua_State* luastate;

		unsigned int VBO, VAO, EBO, texture;
		Shader* shader;

		bool* show_demo_window = new bool(false);
		const int frames = 120;
		float* frametimes;
		double lasttime;
};
