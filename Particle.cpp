//Math Constants
#define _USE_MATH_DEFINES
#include <cmath>
#include "Particle.h"

// default constructor : creates a particle represented by a cube.
 // Notes :
 // - particle rotated so that it is orthogonal to the z axis .
 // - scaled
 // - no shader allocated by default to avoid creating a Shader object for each particle .
 Particle::Particle()
 {
	
	scale(glm::vec3(0.1f, 0.1f, 0.1f));
	rotate((GLfloat)M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
	
		 // set dynamic values
	setAcc(glm::vec3(0.0f, 0.0f, 0.0f));
	setVel(glm::vec3(0.0f, 0.0f, 0.0f));
	
		 // physical properties
	setMass(1.0f);
	setCor(1.0f);
 }

 Particle::~Particle()
{
}