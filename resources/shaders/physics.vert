#version 410
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normalIn;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 rotate;


uniform vec3 eyePosition;

out vec3 normal;
out vec3 normalLocal;

void main()
{
     gl_Position = projection * view * model * vec4(position, 1.0f);  
	 vec4 normal4 = vec4(normalIn, 1.0f);
	 vec4 wNormal = rotate * normal4;
     normal = vec3(wNormal);
     normalLocal = normalIn;
}