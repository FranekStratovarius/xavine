#include <stdio.h>
#include "sprite.hpp"

Sprite::Sprite(
			Shader* shader,
			unsigned int texture,
			glm::vec2 position,
			glm::vec2 size,
			float rotation,
			glm::vec3 color
		){
	this->shader = shader;
	this->texture = texture;
	this->position = position;
	this->size = size;
	this->rotation = rotation;
	this->color = color;
	this->initRenderData();

	/*
	projection = glm::ortho(
		0.0f, 800.0f,
		600.0f, 0.0f,
		-1.0f, 1.0f
	);
	*/
}

Sprite::~Sprite(){
	glDeleteVertexArrays(1, &this->quadVAO);
	delete shader;
}

//void Sprite::draw_sprite(unsigned int texture, glm::vec2 position, glm::vec2 size, float rotate, glm::vec3 color){
void Sprite::draw_sprite(){
	// prepare transformations
	this->shader->use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));  // first translate (transformations are: scale happens first, then rotation, and then final translation happens; reversed order)
	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move origin of rotation to center of quad
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // move origin back

	model = glm::scale(model, glm::vec3(size, 1.0f)); // last scale

	glm::mat4 projection = glm::ortho(
		0.0f, 800.0f,
		600.0f, 0.0f,
		-1.0f, 1.0f
	);
	this->shader->SetMatrix4("projection", projection);
	this->shader->SetMatrix4("model", model);

	// render textured quad
	this->shader->SetVector3f("spriteColor", color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(this->quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Sprite::set_texture(unsigned int texture){
	this->texture = texture;
}
void Sprite::set_position(glm::vec2 position){
	this->position = position;
}
void Sprite::set_size(glm::vec2 size){
	this->size = size;
}
void Sprite::set_rotaton(float rotation){
	this->rotation = rotation;
}
void Sprite::set_color(glm::vec3 color){
	this->color = color;
}

void Sprite::initRenderData(){
	// configure VAO/VBO
	unsigned int VBO;
	float vertices[] = {
		// pos	  // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &this->quadVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(this->quadVAO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
