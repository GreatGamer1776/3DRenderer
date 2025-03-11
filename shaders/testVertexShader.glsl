#version 460 compatibility

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;

out vec3 fragColor;

void main(){  
	//gl_Position.xyz = vertexPosition_modelspace;
	//gl_Position.w = 1.0;
	
	// Output position of the vertex, in clip space: MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	fragColor = vertexColor;
}