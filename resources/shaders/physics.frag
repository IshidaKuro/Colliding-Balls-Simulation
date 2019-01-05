#version 410

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normalIn;

uniform vec4 ambient;
uniform vec3 eyePosition;

in vec3 normal;
in vec3 normalLocal;
out vec4 color;

struct DirectionalLight
{
    vec3 color;
    vec3 direction;
};

/*
This shader applies a Lambert shading model and uses a directional light as single light source.
*/
void main()
{
	// directional light properties
	DirectionalLight light;
	light.color = vec3(0.5f, 0.5f, 0.5f);
	light.direction = vec3(-2.0f, 0.5f, 3.7f);

    // ambient 
    float ambientIntensity = .5f;
    vec3 ambient = light.color * ambientIntensity;

    
    // diffuse
    float diffuseIntensity = 1.0f;
    vec3 diffColour = vec3(1.0f, 0.0f, 0.2f);
    if (normalLocal.y > 0 && normalLocal.x > 0){
        diffColour = vec3(1.0f, 0.0f, 0.2f);
    }
    if (normalLocal.y < 0 && normalLocal.x > 0){
        diffColour = vec3(.2f, 0.0f, 1.0f);
    }
    if (normalLocal.y < 0 && normalLocal.x < 0){
        diffColour = vec3(1.0f, 0.0f, 0.2f);
    }
    if (normalLocal.y > 0 && normalLocal.x < 0){
        diffColour = vec3(.2f, 0.0f, 1.0f);
    }
    
	vec3 diffuse = .2f * max(dot(light.direction, normal),0) * diffColour * diffuseIntensity;
	
	// final colour
    color = vec4(ambient + diffuse, 1.0);
}