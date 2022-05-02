#include <stdio.h>
#include "window.hpp"
#include "sprite.hpp"
#include "texture.hpp"

int main(int, char**){

	//create window
	Window* window;
	try{
		window = new Window(false);
	}catch(const char* message){
		fprintf(stderr,message);
		return 1;
	}

	if(window==nullptr){
		fprintf(stderr,"no window");
	}


	fprintf(stdout, "x: %d, y: %d\n",
		static_cast<float>(window->width),
		static_cast<float>(window->height)
	);
    //load textures
	unsigned int texture = load_texture("assets/textures/meins.jpg");
	//create and initialize shader
	Shader* shader1 = new Shader("assets/shader/vertex.glsl", "assets/shader/fragment.glsl");
	//create sprite
	Sprite* sprite1 = new Sprite(shader1);
	sprite1->set_texture(texture);
	sprite1->set_position(glm::vec2(50.0f, 50.0f));
	sprite1->set_size(glm::vec2(50.0f, 50.0f));
	sprite1->set_color(glm::vec3(1.0, 1.0, 1.0));
	sprite1->set_rotaton(0.0f);

	/*
	Shader* shader2 = new Shader("assets/shader/vertex.glsl", "assets/shader/fragment.glsl");
	Sprite* sprite2 = new Sprite(shader2);
	sprite2->set_texture(texture);
	sprite2->set_position(glm::vec2(150.0f, 50.0f));
	sprite2->set_size(glm::vec2(30.0f, 30.0f));
	sprite2->set_color(glm::vec3(1.0, 0.0, 0.0));
	sprite2->set_rotaton(45.0f);
	*/

	//create spritelist
	Sprite** sprites = new Sprite*[2];
	sprites[0] = sprite1;
	//sprites[1] = sprite2;

	float x = 0.0f;

	//main loop
	while(!window->closed()){
		window->poll_events();
		window->render(sprites, 1);
		sprite1->set_position(glm::vec2(x, 50.0f));
		x+=0.1f;
	}

	delete window;

	return 0;
}
