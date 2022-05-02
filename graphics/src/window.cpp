#include <stdio.h>
#include <stdlib.h>
#include "window.hpp"
#include "texture.hpp"
#include "linmath.h"
#include "my_imgui.hpp"

static void glfw_error_callback(int error, const char* description){
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	Window* my_window = (Window*)glfwGetWindowUserPointer(window);
	if(my_window){
		my_window->my_key_callback(glfwGetKeyName(key,scancode),action);
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	Window* my_window = (Window*)glfwGetWindowUserPointer(window);
	my_window->width = width;
	my_window->height = height;
}

Window::Window(bool fullscreen){
	// Setup window

	glfwSetErrorCallback(glfw_error_callback);
	if(!glfwInit()){
		throw "GLFW initialization failed";
	}

	// Decide GL+GLSL versions
#if defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);			// 3.0+ only
#endif

	// Create window with graphics context
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if(fullscreen){
		window = glfwCreateWindow(mode->width,mode->height,"xavine",monitor,NULL);
	}else{
		window = glfwCreateWindow(mode->width,mode->height,"xavine",NULL,NULL);
	}
	if(window == NULL){
		throw "could not create GLFW window";
	}

	this->width = mode->width;
	this->height = mode->height;

	glfwSetWindowUserPointer(window,this);

	glfwSetKeyCallback(window,key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwMakeContextCurrent(window);
	gladLoadGL(glfwGetProcAddress);
	glfwSwapInterval(1); // Enable vsync

	//glViewport(0, 0, width, height);

	setup_my_imgui(glsl_version,window);

	frametimes = new float[frames];
	frametimes[0] = 0;
	frametimes[1] = 1.f/30.0f;
	lasttime = glfwGetTime();

	/*
	luastate = load_lua();
	if(!luastate){
		throw "luastate not initialized";
	}
	*/
}

Window::~Window(){
	delete show_demo_window;
	delete[] frametimes;

	//close_lua(luastate);

	cleanup_my_imgui();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::render(){
	this->render(nullptr, 0);
}

void Window::render(Sprite** sprites, unsigned int sprite_count){
	for(int i=2;i<frames-1;i++){
			frametimes[i]=frametimes[i+1];
	}
	double newtime = glfwGetTime();
	frametimes[frames-1] = (float)newtime-lasttime;
	lasttime = newtime;

	//clear screen
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for(int i = 0;i < sprite_count;i++){
		sprites[i]->draw_sprite();
	}

	render_my_imgui(frametimes, frames, show_demo_window);

	//show screen
	glfwSwapBuffers(window);
}

bool Window::closed(){
	return glfwWindowShouldClose(window);
}

void Window::poll_events(){
	glfwPollEvents();

	//run_lua(luastate);
}

void Window::my_key_callback(const char* key,int action){
	//lua_key_pressed(luastate,key,action);
}
