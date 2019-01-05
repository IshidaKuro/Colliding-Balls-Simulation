#include "RigidBody.h"
#include <cmath>
#define _USE_MATH_DEFINES

RigidBody::RigidBody()
{
	
		
		
		setAngAccl(glm::vec3(0,0,0));
		setAngVel(glm::vec3(0,0,0));
		
		scale(glm::vec3(1,1,1));

		setAcc(glm::vec3(0.0f, 0.0f, 0.0f));
		setVel(glm::vec3(0.0f, 0.0f, 0.0f));

		// physical properties
		setMass(2.0f);
		setCor(1.0f);

		
	
}


RigidBody::~RigidBody()
{
}
