#pragma once
#include <glad/gl.h>

namespace Shader{
	GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
}
