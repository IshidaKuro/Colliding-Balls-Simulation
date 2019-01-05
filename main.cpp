#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>
#include <iostream>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"
#include "glm/gtx/string_cast.hpp"

// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Body.h"
#include "Particle.h"
#include "Force.h"
#include "RigidBody.h"


// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//declare the number of spheres in the simulation
const int numberOfSpheres = 3000;

// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 30.0f, 50.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	// scale it up x30
	plane.scale(glm::vec3(300.0f, 300.0f, 300.0f));
	Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);

	Gravity *g = new Gravity;
	
	//random number generation
	std::default_random_engine generator;
	std::uniform_real_distribution<float> distribution(-290, 290);
	std::uniform_real_distribution<float> speedDistribution(-20, 20);
	
	

	//create a sphere
	RigidBody rb[numberOfSpheres];
	for (int i = 0; i < numberOfSpheres; i++)
	{
		rb[i].setMesh(Mesh("resources/models/sphere2.obj"));


		Shader rbShader = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
		rb[i].getMesh().setShader(rbShader);

		// rigid body motion values
		rb[i].translate(glm::vec3(distribution(generator), 1.0, distribution(generator)));
		rb[i].setVel(glm::vec3(speedDistribution(generator), 0.0, speedDistribution(generator)));
		
		
		rb[i].setCor(1.0f);
		
		float radius = 1;
		rb[i].scale(glm::vec3(radius, radius, radius));

		float m = rb[i].getMass(); // get the mass of the object


		//calculate the inertia tensor for the object
		glm::mat3x3 inertiaTensor = glm::mat3x3((2 / 5)* m * pow(radius, 2), 0, 0,
			0, (2 / 5)* m * pow(radius, 2), 0,
			0, 0, (2 / 5)* m * pow(radius, 2));

	}
	

	
	
	
	
	///// 
	double t = 0.0;
	double dt = 0.01;
	double initialTime = glfwGetTime();
	double currentTime = 0.0;
	double accumulator = 0.0;
	double newTime;

	

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		
		app.showFPS();
		// time stuff
		newTime = glfwGetTime()- initialTime;
		t = newTime - currentTime;
		currentTime = newTime;

		accumulator += t;

		while (accumulator >= dt)
		{

			for (int i = 0; i < numberOfSpheres; i++)
			{
				// integration ( rotation )
				rb[i].setAngVel(rb[i].getAngVel() + dt * rb[i].getAngAcc());
				// create skew symmetric matrix for w
				glm::mat3 angVelSkew = glm::matrixCross3(rb[i].getAngVel());
				// create 3x3 rotation matrix from rb[i] rotation matrix
				glm::mat3 R = glm::mat3(rb[i].getRotate());
				// update rotation matrix
				R += dt * angVelSkew *R;
				R = glm::orthonormalize(R);
				rb[i].setRotate(glm::mat4(R));
				//set the inverse inertia of the object


				//values required for impulses
				float jr;
				float e = rb[i].getCor();
				glm::vec3 vR = rb[i].getVel();
				

				glm::vec4 rbPos;
				std::vector<Vertex> verti = rb[i].getMesh().getVertices();







				rb[i].setAcc(rb[i].applyForces(rb[i].getPos(), rb[i].getVel(), t, dt));

				rb[i].setVel(rb[i].getVel() + (rb[i].getAcc()*dt));

				//set the rigid body's new position

				rb[i].setPos(rb[i].getPos() + rb[i].getVel()*dt);


				rb[i].setAngVel(glm::vec3(rb[i].getVel().z, -rb[i].getVel().y, -rb[i].getVel().x));

				//--------------------------------------------------

				//collisions
				{
					
						//collision with plane

						if (rb[i].getPos().y < 1)
						{
							rb[i].setPos(1, 1);
							rb[i].setVel(rb[i].getVel() * glm::vec3(1, -1, 1));

						}

						if ((rb[i].getPos().x < -299) || (rb[i].getPos().x > 299))
						{
							if (rb[i].getPos().x < -299)
							{
								rb[i].setPos(0, -299);
							}
							else
							{
								rb[i].setPos(0, 299);
							}
							rb[i].setVel(rb[i].getVel() * glm::vec3(-1, 1, 1));

						}
						if ((rb[i].getPos().z < -299) || (rb[i].getPos().z > 299))
						{
							if (rb[i].getPos().z < -299)
							{
								rb[i].setPos(2, -299);
							}
							else
							{
								rb[i].setPos(2, 299);
							}
							rb[i].setVel(rb[i].getVel() * glm::vec3(1, 1, -1));
						}

						//collisions with other spheres
						for (int j = i + 1; j < numberOfSpheres; j++)
						{
							if (rb[j].getPos() != rb[i].getPos())
							{
								//if the tentative distance between the two center points of the spheres is less than 2 a collision must take place
								if ((pow((rb[j].getPos().x - rb[i].getPos().x), 2) + pow((rb[j].getPos().z - rb[i].getPos().z), 2)) < 4)
								{
									//resolve any overlap
									float pointDistance = sqrt(pow((rb[j].getPos().x - rb[i].getPos().x), 2) + pow((rb[j].getPos().z - rb[i].getPos().z), 2));
									float overlap = 0.5f * (pointDistance - 2);
									rb[j].setPos(rb[j].getPos() - overlap * (rb[j].getPos() - rb[i].getPos()) / pointDistance);
									rb[i].setPos(rb[i].getPos() + overlap * (rb[j].getPos() - rb[i].getPos()) / pointDistance);

									//find the normal for the collision


									glm::vec3 collisionNormal = glm::normalize(rb[j].getPos() - rb[i].getPos());

									//generate a collission impulse


									float a1 = glm::dot(rb[j].getVel(), collisionNormal);
									float a2 = glm::dot(rb[i].getVel(), collisionNormal);

									float P = (2.0 * (a1 - a2)) / (rb[j].getMass() + rb[i].getMass());

									rb[j].setVel(rb[j].getVel() - P * rb[i].getMass() * collisionNormal);
									rb[i].setVel(rb[i].getVel() + P * rb[j].getMass() * collisionNormal);


								}
							}
						}
				}



			}

			accumulator -= dt;
			
		}
			/*
			**	INTERACTION
			*/
			// Manage interaction
			app.doMovement(t);
		

		/*
		**	SIMULATION
		*/
		


		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		// draw ground plane
		app.draw(plane);
		//draw rigid body
		for each (RigidBody rb in rb)
		{


			app.draw(rb.getMesh());
		}
		//app.draw(rb.getMesh());
		app.display();
		

		
	}

	app.terminate();

	return EXIT_SUCCESS;
}

