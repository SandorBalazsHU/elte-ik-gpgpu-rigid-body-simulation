/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
	---
	The dynamic barrier modell generator.
*/

#include "simulation.h"

//Create the barrier box modell dinamically by barrierSize
void Simulation::barrierBuilder() {

	barrierShift = glm::vec3(0, -boxSize + ((boxSize) / 6) + 0.01f, 0);
	barrierSize = boxSize / 6;

	barrierCoordinates.BufferData(
		std::vector<glm::vec3>{
			glm::vec3(-barrierSize, -barrierSize, -barrierSize),
			glm::vec3(barrierSize, -barrierSize, -barrierSize),
			glm::vec3(-barrierSize, -barrierSize, barrierSize),
			glm::vec3(barrierSize, -barrierSize, barrierSize),
			glm::vec3(-barrierSize, barrierSize, -barrierSize),
			glm::vec3(barrierSize, barrierSize, -barrierSize),
			glm::vec3(barrierSize, barrierSize, barrierSize),
			glm::vec3(-barrierSize, barrierSize, barrierSize)});

	barrierNormals.BufferData(
		std::vector<glm::vec3>{
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0),
			glm::vec3(0, 1, 0)});

	barrierTextures.BufferData(
		std::vector<glm::vec2>{
		glm::vec2(0, 0),
			glm::vec2(1, 0),
			glm::vec2(0, 1),
			glm::vec2(1, 1),
			glm::vec2(0, 0),
			glm::vec2(1, 0),
			glm::vec2(0, 1),
			glm::vec2(1, 1),});

	barrierIndices.BufferData(
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
			2, 6, 7,
			2, 6, 7,
			2, 6, 3});

	barrier.Init(
		{
			{ CreateAttribute<0, glm::vec3, 0, sizeof(glm::vec3)>, barrierCoordinates },
			{ CreateAttribute<1, glm::vec3, 0, sizeof(glm::vec3)>, barrierNormals },
			{ CreateAttribute<2, glm::vec2, 0, sizeof(glm::vec2)>, barrierTextures }
		}, barrierIndices);
}