#version 330 core
layout(location = 0) in vec3 location;

out vec2 uv;

void main()
{
	gl_Position = vec4(location, 1);

	uv = (location.xy + vec2(1, 1)) / 2.0;
}