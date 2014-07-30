#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 uv;
out vec3 in_position_cameraspace;
out vec3 normal_cameraspace;

uniform mat4 mvp;
uniform mat4 m;
uniform mat4 v;

void main()
{
	gl_Position = mvp * vec4(vertexPosition_modelspace, 1);

	in_position_cameraspace = (v * m * vec4(vertexPosition_modelspace, 1)).xyz;

	normal_cameraspace = (v * m * vec4(vertexNormal_modelspace, 0)).xyz;

	uv = vertexUV;
}