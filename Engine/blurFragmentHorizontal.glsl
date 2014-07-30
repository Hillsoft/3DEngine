#version 330 core

uniform sampler2D image;
uniform float pixSize;

in vec2 uv;

layout(location = 0) out vec3 color;

void main()
{
	vec3 sum = vec3(0.0);

	sum += texture2D(image, vec2(uv.x, uv.y - 4.0 * pixSize)).rgb * 0.05;
	sum += texture2D(image, vec2(uv.x, uv.y - 3.0 * pixSize)).rgb * 0.09;
	sum += texture2D(image, vec2(uv.x, uv.y - 2.0 * pixSize)).rgb * 0.12;
	sum += texture2D(image, vec2(uv.x, uv.y - pixSize)).rgb * 0.15;
	sum += texture2D(image, vec2(uv.x, uv.y)).rgb * 0.16;
	sum += texture2D(image, vec2(uv.x, uv.y + pixSize)).rgb * 0.15;
	sum += texture2D(image, vec2(uv.x, uv.y + 2.0 * pixSize)).rgb * 0.12;
	sum += texture2D(image, vec2(uv.x, uv.y + 3.0 * pixSize)).rgb * 0.09;
	sum += texture2D(image, vec2(uv.x, uv.y + 4.0 * pixSize)).rgb * 0.05;

	color = sum;
}