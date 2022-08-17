#include <stdio.h>
#include <string.h>

#include <bgfx/bgfx.h>
#include "graphics.h"

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
		case bgfx::RendererType::Nvn:        break;
		case bgfx::RendererType::WebGPU:     break;
		case bgfx::RendererType::Count:      break;
	}

	size_t shaderLen = strlen(shaderPath);
	size_t fileLen = strlen(FILENAME);
	//printf("shaderLen = %d, fileLen = %d\n", shaderLen, fileLen);
	char *filePath = (char *)malloc(shaderLen + fileLen);
	strcpy (filePath, shaderPath);
	strcat (filePath, FILENAME);

	FILE *file = fopen(filePath, "rb");
	//printf("Loading shader: %s\n", filePath);
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
	fread(mem->data, 1, fileSize, file);
	mem->data[mem->size - 1] = '\0';
	fclose(file);

	return bgfx::createShader(mem);
}