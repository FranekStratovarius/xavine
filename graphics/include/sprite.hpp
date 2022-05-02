#ifndef SPRITE_HPP
#define SPRITE_HPP
#include "glad/gl.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "shader.hpp"

class Sprite{
	public:
		Sprite(
			Shader* shader,
			unsigned int texture = 0,
			glm::vec2 position = glm::vec2(10.0f, 10.0f),
			glm::vec2 size = glm::vec2(10.0f, 10.0f),
			float rotation = 0.0f,
			glm::vec3 color = glm::vec3(1.0f)
		);
		~Sprite();
		void draw_sprite();

		void set_texture(unsigned int texture);
		void set_position(glm::vec2 position);
		void set_size(glm::vec2 size = glm::vec2(10.0f, 10.0f));
		void set_rotaton(float rotation = 0.0f);
		void set_color(glm::vec3 color = glm::vec3(1.0f));
	private:
		void initRenderData();

		Shader* shader;
		unsigned int quadVAO;

		unsigned int texture;
		glm::vec2 position;
		glm::vec2 size;
		float rotation;
		glm::vec3 color;
};
#endif
