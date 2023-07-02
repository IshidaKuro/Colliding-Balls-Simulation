#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>
#include <iostream>
#include <omp.h>
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

using namespace std;
// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

//sphere variables
constexpr int numberOfSpheres = 3310;
constexpr float radius = 1.0f;

constexpr int planeScale  = 200;
constexpr int gridXY = 40;

//let the compiler do some of the math for us
constexpr float planeMinusRadius = planeScale - radius;
constexpr float radiusMinusPlane = radius - planeScale;
constexpr float poolModifier = 2 * planeScale / gridXY;
constexpr float twiceRadius = 2 * radius;
constexpr float collissionDistance = twiceRadius * twiceRadius;

vector<vector<vector<int>>> ballPool(gridXY, vector<vector<int>>(gridXY, vector<int>()));
const static vector<vector<vector<int>>> emptyBallPool(gridXY,vector<vector<int>>(gridXY,vector<int>()));

// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 100.0f, 100.0f));

	// create ground plane
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	// scale it up
	plane.scale(glm::vec3(planeScale, planeScale, planeScale));
	//set shader
	const Shader lambert = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	plane.setShader(lambert);

	const Gravity *g = new Gravity;

	//random number generation
	default_random_engine generator;
	uniform_real_distribution<float> distribution(1 - planeScale, planeScale - 1);
	uniform_real_distribution<float> speedDistribution(-30, 30);

	//shaders and textures
	Shader rbShader = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	Mesh sphere = Mesh("resources/models/sphere2.obj");

	

	//create spheres
	RigidBody rb[numberOfSpheres];
	for (int i = numberOfSpheres - 1; i >= 0; i--)
	{
		rb[i].setMesh(sphere);
		rb[i].getMesh().setShader(rbShader);

		// rigid body motion values
		rb[i].translate(glm::vec3(distribution(generator), 1.0, distribution(generator)));
		rb[i].setVel(glm::vec3(speedDistribution(generator), 0.0, speedDistribution(generator)));


		rb[i].setCor(1.0f);

		rb[i].scale(glm::vec3(radius, radius, radius));
	}
	
	/////time variables 
	double t = 0.0;
	double dt;
	double initialTime = glfwGetTime();
	double currentTime = 0.0;
	double accumulator = 0.0;
	double newTime;

	ballPool.reserve(numberOfSpheres);
	float LHigh, MHigh, LLow, MLow;

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		app.showFPS();
		// time stuff
		newTime = glfwGetTime() - initialTime;
		dt = newTime - currentTime;
		currentTime = newTime;

		accumulator += dt;

		while (accumulator >= dt)
		{
			//clear ball pools
			ballPool = emptyBallPool;

			//for each sphere in the simulation
			for (int i = numberOfSpheres - 1; i >= 0; i--)
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
				
				//values required for impulses

				float e = rb[i].getCor();
				glm::vec3 vR = rb[i].getVel();

				vector<Vertex> verti = rb[i].getMesh().getVertices();

				//apply forces to generate acceleration
				rb[i].setAcc(rb[i].applyForces(rb[i].getPos(), rb[i].getVel(), t, dt));

				//update speed
				rb[i].setVel(rb[i].getVel() + (rb[i].getAcc()*dt));

				//set the rigid body's new position
				rb[i].setPos(rb[i].getPos() + rb[i].getVel()*dt);

				//set new angular velocity (rotation speed)
				rb[i].setAngVel(glm::vec3(rb[i].getVel().z, -rb[i].getVel().y, -rb[i].getVel().x));

				//--------------------------------------------------

				//collisions
				{
					//frequently used variables during collision detection
					float ix, iz, jx, jz;
					ix = rb[i].getPos().x;
					iz = rb[i].getPos().z;

					//if the ball is at a plane boundary
					if ((ix < radiusMinusPlane) || (ix > planeMinusRadius))
					{
						if (ix < radiusMinusPlane)
						{
							rb[i].setPos(0, radiusMinusPlane);
						}
						else
						{
							rb[i].setPos(0, planeMinusRadius);
						}
						rb[i].setVel(rb[i].getVel() * glm::vec3(-1, 1, 1));
					}
					if ((iz < radiusMinusPlane) || (iz > planeMinusRadius))
					{
						if (iz < radiusMinusPlane)
						{
							rb[i].setPos(2, radiusMinusPlane);
						}
						else
						{
							rb[i].setPos(2, planeMinusRadius);
						}
						rb[i].setVel(rb[i].getVel() * glm::vec3(1, 1, -1));
					}

					//assign ball to appropriate pool and perfom collission check
					LLow = -planeScale;
					for (int l = 0; l < gridXY; l++)
					{
						LHigh = LLow + poolModifier;
					
						if ((ix - radius < LHigh) && (ix + radius> LLow))
						{
							MLow = -planeScale;
							for (int m = 0; m < gridXY; m++)
							{
								MHigh = MLow + poolModifier;
								if ((iz - radius < MHigh) && (iz + radius > MLow))
								{
									//check for collissions before adding to the list in order to prevent checking if the ball collides with itself
									//checking for collissions as we add also means that no redundant comparisons will be made

									//check if the ball collides with any others on this list

									//*** TO DO - pull collission check out of for each loop and execute on multiple threads

									for (int j : ballPool[l][m])
									{

										jx = rb[j].getPos().x;
										jz = rb[j].getPos().z;

										float pointDistance = glm::fastSqrt(((jx - ix) * (jx - ix)) + ((jz - iz) * (jz - iz)));
										
										//if the distance between the two center points of the spheres is less than twice the radius, a collission must take place
										if (pointDistance < twiceRadius)
										{
											//resolve any overlap
											
											float overlap = 0.5f * (pointDistance - twiceRadius);
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

									//add the ball to the list
									ballPool[l][m].emplace_back(i);
								}
								MLow = MHigh;
							}
						
						}

						LLow = LHigh;
					}


				}
			}

				accumulator -= dt;
		}
			
			// Manage interaction
			app.doMovement(t);

			// clear buffer
			app.clear();
			// draw ground plane
			app.draw(plane);
			//draw rigid body
			for (RigidBody r : rb)
			{
				app.draw(r.getMesh());
			}
			app.display();
		}

		app.terminate();
		return EXIT_SUCCESS;
	}

