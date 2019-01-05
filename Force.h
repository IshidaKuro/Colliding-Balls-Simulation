#pragma once
#include <glm/glm.hpp>
#include <iostream>

class Body;

class Force
{
public:
	Force() {}
	~Force() {}

	virtual glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);
};

/*
**  Gravity Class
*/
class Gravity : public Force {

public:
		//constructors
		Gravity() {}
		Gravity(const glm::vec3 &gravity) { m_gravity = gravity; }

	//get and set methods
	glm::vec3 getGravity() const { return m_gravity; }
	void setGravity(glm::vec3 gravity) { m_gravity = gravity; }

	//physics
	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:
	glm::vec3 m_gravity = glm::vec3(0.0f, -9.8f, 0.0f);
};

/*
**	Drag Class 
*/
class Drag : public Force {
public:
	Drag() {}
	//physics
	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:

};

//Hooke Class
class Hooke : public Force {
 public:
	 Hooke() {}
	 Hooke(Body * b1, Body * b2, float ks, float kd, float rest) {
		 m_ks = ks; m_kd = kd; m_rest = rest; m_b1 = b1; m_b2 = b2;
		}
	
		 // get and set methods
		 // you can write these yourself as necessary
		
		 // physics
		 glm::vec3 apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel);
	
 private:
	 float m_ks; // spring stiffness
	 float m_kd; // damping coefficient
	 float m_rest; // spring rest length
	
	 Body * m_b1; // pointer to the body connected to one extremity of the spring
	 Body * m_b2; // pointer to the body connected to the other extremity
	
};

//Impulse Class
class Impulse : public Force{
public:
	Impulse() {}
	Impulse(glm::vec3 cv, glm::vec3 nv) 
	{
		m_cv = cv;
		m_nv = nv;
	}

	//physics
	glm::vec3 apply(float mass, const glm::vec3 & pos, const glm::vec3 & vel);

private:
	glm::vec3 m_cv; //current velocity
	glm::vec3 m_nv; //new velocity

};