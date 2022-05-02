#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "glad/gl.h"
#include "shader.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath){
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
	try{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode   = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}catch (std::ifstream::failure& e){
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();

	//fprintf(stdout, "vertex Shader: %s\n", vShaderCode);
	//fprintf(stdout, "fragment Shader: %s\n", fShaderCode);

	unsigned int vertex, fragment;

	//compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");

	//compile fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");

	//link shader program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	//delete shaders after linking
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use(){
	glUseProgram(ID);
}

void Shader::setBool(const char* &name, bool value){
	glUniform1i(glGetUniformLocation(ID, name), (int)value);
}

void Shader::setInt(const char* &name, int value){
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::setFloat(const char* &name, float value){
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::setFloat(const char* name, float value){
	glUniform1f(glGetUniformLocation(ID, name), value);
}

void Shader::SetInteger(const char* name, int value){
	glUniform1i(glGetUniformLocation(ID, name), value);
}

void Shader::SetVector2f(const char* name, float x, float y){
	glUniform2f(glGetUniformLocation(ID, name), x, y);
}

void Shader::SetVector2f(const char* name, const glm::vec2 &value){
	glUniform2f(glGetUniformLocation(ID, name), value.x, value.y);
}

void Shader::SetVector3f(const char* name, float x, float y, float z){
	glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::SetVector3f(const char* name, const glm::vec3 &value){
	glUniform3f(glGetUniformLocation(ID, name), value.x, value.y, value.z);
}

void Shader::SetVector4f(const char* name, float x, float y, float z, float w){
	glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
}

void Shader::SetVector4f(const char* name, const glm::vec4 &value){
	glUniform4f(glGetUniformLocation(ID, name), value.x, value.y, value.z, value.w);
}

void Shader::SetMatrix4(const char* name, const glm::mat4 &matrix){
	glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, false, glm::value_ptr(matrix));
}

void Shader::checkCompileErrors(unsigned int shader, const char* type){
	int success;
	char infolog[1024];
	if(type != "PROGRAM"){
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if(!success){
			glGetShaderInfoLog(shader, 1024, nullptr, infolog);
			fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\n--------------------------\n", type, infolog);
		}
	}else{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if(!success){
			glGetShaderInfoLog(shader, 1024, nullptr, infolog);
			fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\n--------------------------\n", type, infolog);
		}
	}
}
