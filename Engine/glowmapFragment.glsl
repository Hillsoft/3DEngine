#version 330 core

uniform sampler2D image;

in vec2 uv;

layout(location = 0) out vec3 color;

void main()
{
	vec3 tempCol = texture2D(image, uv).rgb;
	if (tempCol.r + tempCol.g + tempCol.b > 2)
	{
		color = tempCol;
	}
	else
	{
		color = vec3(0);
	}
}