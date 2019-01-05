#include <iostream>
#include <cmath>
#include "Force.h"
#include "Body.h"
#include "glm/ext.hpp"


glm::vec3 Force::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {
	return glm::vec3(0.0f);
}


/*
** GRAVITY
*/
 glm::vec3 Gravity::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {
	 // complete . Should return the gravity force
	 
	 glm::vec3 Fg = m_gravity * mass;
	 return Fg;
}

/*
 ** DRAG
*/
 glm::vec3 Drag::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {
	 // complete . Should return the aerodynamic drag force
	 return glm::vec3(0, 0, 0);
 }
 /*
 ** HOOKE ’S LAW
 */
  glm::vec3 Hooke::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {
	  // fSpring = -(SPRING STIFFNESS) * (DISPLACEMENT)
	  // fDamp = -(DAMPING COEFFICIENT) * (VELOCITY)
	  
	  glm::vec3 fHooke;
	  glm::vec3 difference = (m_b2->getPos() - m_b1->getPos());
	  float length = glm::distance(m_b2->getPos(), m_b1->getPos());
	  glm::vec3 e = glm::normalize(difference);
	  float displacement = m_rest - glm::distance(m_b2->getPos(), m_b1->getPos());
	  float springStiffness = m_ks;
	  float v1 = glm::dot(e,m_b1->getVel());
	  float v2 = glm::dot(e,m_b2->getVel());
	  if ((v1 = 0))
	  {
		  v1 = -v2;
	  
	  }
	  float fsd = -(m_ks * displacement) - (m_kd * (v1 - v2));
	  //add the forces to get the hooke force
	  glm::vec3 f1 = fsd * e;
	  glm::vec3 f2 = -f1;
	  if (m_rest < displacement)
	  {
		  fHooke = f1;
	  }
	  else
	  {
		  fHooke = f2;
	  }
	  return fHooke;
 }

  glm::vec3 Impulse::apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel) {

	  glm::vec3 fImpulse;
	  glm::vec3 currentVelocity;
	  glm::vec3 newVelocity;


	  fImpulse = (mass*newVelocity) - (mass*currentVelocity);

	  return fImpulse;
  }