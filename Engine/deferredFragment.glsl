#version 330 core

in vec2 uv;

uniform sampler2D position_cameraspaceTex;
uniform sampler2D diffuseTex;
uniform sampler2D specularTex;
uniform sampler2D normalTex;

// uniform mat4 v;
uniform vec3 lightPosition_cameraspace;
uniform vec3 lightColor;
uniform float lightPower;

layout(location = 0) out vec3 color;

void main()
{
	vec3 position_cameraspace = texture(position_cameraspaceTex, uv).xyz;
	vec3 normal = texture(normalTex, uv).xyz;

	vec3 materialDiffuseColor = texture(diffuseTex, uv).rgb;
	vec3 ambientColor = materialDiffuseColor * lightColor * 0.2;
	materialDiffuseColor *= 0.9;
	vec3 materialSpecularColor = texture(specularTex, uv).rgb;

	vec3 eyeDirection_cameraspace = vec3(0, 0, 0) - position_cameraspace;
	vec3 lightDirection_cameraspace = lightPosition_cameraspace - position_cameraspace;
	float lightDistance = length(lightDirection_cameraspace);

	vec3 n = normalize(normal);
	vec3 l = normalize(lightDirection_cameraspace);

	float cosTheta = clamp(dot(n, l), 0, 1);

	vec3 diffuseColor = materialDiffuseColor * lightColor * lightPower * cosTheta / (lightDistance * lightDistance);
	ambientColor *= lightPower / (lightDistance * lightDistance);
	
	vec3 e = normalize(eyeDirection_cameraspace);
	vec3 r = reflect(-l, n);

	float cosAlpha = clamp(dot(e, r), 0, 1);
	
	vec3 specularColor = materialSpecularColor * lightColor * lightPower * pow(cosAlpha, 20) / (lightDistance * lightDistance);

	color = diffuseColor + ambientColor + specularColor;
}