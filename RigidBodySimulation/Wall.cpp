/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
	---
	The dynamic barrier modell generator.
*/

#include "simulation.h"

//Create the wall box modell dinamically by boxSize
void Simulation::wallBuilder()
{
	wallCoordinates.BufferData(
			std::vector<glm::vec3>{
			glm::vec3(-boxSize, -boxSize, -boxSize),
			glm::vec3( boxSize, -boxSize, -boxSize), 
			glm::vec3(-boxSize, -boxSize,  boxSize), 
			glm::vec3( boxSize, -boxSize,  boxSize),  
			glm::vec3(-boxSize,  boxSize, -boxSize),	
			glm::vec3( boxSize,  boxSize, -boxSize),
			glm::vec3( boxSize,  boxSize,  boxSize), 
			glm::vec3(-boxSize,  boxSize,  boxSize)});

	wallNormals.BufferData(
		std::vector<glm::vec3>{
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0)});

	wallTextures.BufferData(
			std::vector<glm::vec2>{
			glm::vec2(0, 0),
			glm::vec2(1, 0),
			glm::vec2(0, 1),
			glm::vec2(1, 1),
			glm::vec2(0, 0),
			glm::vec2(1, 0),
			glm::vec2(0, 1),
			glm::vec2(1, 1)});

	wallIndices.BufferData(
		std::vector<int>{
			0, 1, 2,
			2, 1, 3,
			2, 0, 4,
			4, 7, 2,
			4, 0, 1,
			4, 5, 1,
			3, 1, 5,
			3, 6, 5,
			2, 3, 6,
			2, 6, 7});

	wall.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, wallCoordinates },
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, wallNormals },
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, wallTextures }
		}, wallIndices);
}