#version 330 core
in vec2 uv;
in vec3 in_position_cameraspace;
in vec3 normal_cameraspace;

layout(location = 0) out vec3 position_cameraspace;
layout(location = 1) out vec3 color;
layout(location = 2) out vec3 specular;
layout(location = 3) out vec3 normal;

uniform sampler2D textureSampler;

void main()
{
	position_cameraspace = in_position_cameraspace;
	color = vec3(1); // color = texture2D(textureSampler, uv).rgb;
	specular = vec3(0.5, 0.5, 0.5);
	normal = normal_cameraspace;
}