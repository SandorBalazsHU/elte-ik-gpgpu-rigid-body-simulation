/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
	---
	The CPU collision and reflection implemenatation.
	Implement the ball to wall, ball to barrier and ball to ball collision.
*/

#include "Simulation.h"

//The full CPU Collision implementation
void Simulation::Collision_CPU(size_t i) {
	velocities[i] = velocities[i] - gravity;
	velocities[i] = velocities[i] * resistance;
	positions[i]  = positions[i] + velocities[i];
	if (ballCollisionRun) ballCollision(i);
	if(barrierIsOn) barrierCollision(i);
	wallCollision(i);
}

//Handle the ball to wall collision and refraction
void Simulation::wallCollision(size_t i) {
	glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);

	if (positions[i].y <= -boxSize + 1.0f) {
		normal = glm::vec3(0.0f, 1.0f, 0.0f);
		positions[i].y = -boxSize + 1.01f;
	}
	if (positions[i].y >= boxSize - 1.0f) {
		normal = glm::vec3(0.0f, -1.0f, 0.0f);
		positions[i].y = boxSize - 1.01f;
	}
	if (positions[i].x <= -boxSize + 1.0f) {
		normal = glm::vec3(1.0f, 0.0f, 0.0f);
		positions[i].x = -boxSize + 1.01f;
	}
	if (positions[i].x >= boxSize - 1.0f) {
		normal = glm::vec3(-1.0f, 0.0f, 0.0f);
		positions[i].x = boxSize - 1.01f;
	}
	if (positions[i].z <= -boxSize + 1.0f) {
		normal = glm::vec3(0.0f, 0.0f, 1.0f);
		positions[i].z = -boxSize + 1.01f;
	}
	if (positions[i].z >= boxSize - 1.0f) {
		normal = glm::vec3(0.0f, 0.0f, -1.0f);
		positions[i].z = boxSize - 1.01f;
	}

	if (normal != glm::vec3(0.0f, 0.0f, 0.0f)) {
		velocities[i] = glm::reflect(velocities[i], normal);
		velocities[i] = ((velocities[i] * resistance) * resistance) * resistance;	//Plus resistance
	}
}

//Handle the ball to marrier collision and refraction
void Simulation::barrierCollision(size_t i) {
	glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);

	barrierSize = boxSize / 6;

	if (positions[i].y <= (-boxSize + barrierSize*2.0f) + 1.0f &&
	   (positions[i].x >= -barrierSize + barrierShift.x) && (positions[i].x <= barrierSize + barrierShift.x) &&
	   (positions[i].z >= -barrierSize + barrierShift.z) && (positions[i].z <= barrierSize + +barrierShift.z)) {
		normal = glm::vec3(0.0f, 1.0f, 0.0f); 
		positions[i].y = (-boxSize + barrierSize*2.0f) + 1.01f;
	}
	if (positions[i].y < (-boxSize + barrierSize * 2.0f) &&
	   (positions[i].x > -barrierSize + barrierShift.x - 1.0f) && (positions[i].x < -barrierSize + barrierShift.x) &&
	   (positions[i].z > -barrierSize + barrierShift.z) && (positions[i].z < barrierSize + +barrierShift.z)) {
		normal = glm::vec3(1.0f, 0.0f, 0.0f);
		positions[i].x = -barrierSize + barrierShift.x - 1.01f;
	}
	if (positions[i].y < (-boxSize + barrierSize * 2.0f) &&
		(positions[i].x < barrierSize + barrierShift.x + 1.0f) && (positions[i].x > barrierSize + barrierShift.x) &&
		(positions[i].z > -barrierSize + barrierShift.z) && (positions[i].z < barrierSize + +barrierShift.z)) {
		normal = glm::vec3(-1.0f, 0.0f, 0.0f);
		positions[i].x = barrierSize + barrierShift.x + 1.02f;
	}

	if (positions[i].y < (-boxSize + barrierSize * 2.0f) &&
		(positions[i].z > -barrierSize + barrierShift.z - 1.0f) && (positions[i].z < -barrierSize + barrierShift.z) &&
		(positions[i].x > -barrierSize + barrierShift.x) && (positions[i].x < barrierSize + +barrierShift.x)) {
		normal = glm::vec3(0.0f, 0.0f, 1.0f);
		positions[i].z = -barrierSize + barrierShift.z - 1.01f;
	}
	if (positions[i].y < (-boxSize + barrierSize * 2.0f) &&
		(positions[i].z < barrierSize + barrierShift.z + 1.0f) && (positions[i].z > barrierSize + barrierShift.z) &&
		(positions[i].x > -barrierSize + barrierShift.x) && (positions[i].x < barrierSize + +barrierShift.x)) {
		normal = glm::vec3(0.0f, 0.0f, -1.0f);
		positions[i].z = barrierSize + barrierShift.z + 1.02f;
	}

	if (normal != glm::vec3(0.0f, 0.0f, 0.0f)) {
		velocities[i] = glm::reflect(velocities[i], normal);
		velocities[i] = ((velocities[i] * resistance) * resistance) * resistance;	//Plus resistance
	}
}

//Handle the ball to ball collision and reflection
void Simulation::ballCollision(size_t i) {
	for (size_t j = 0; j < numberOfBalls; j++) {
		if (i != j && collisionCheck[j]) {
			float distance = glm::distance(positions[i], positions[j]);
			if (distance < 2.0f)
			{
				//Bcouse zero division.
				if (distance <= 0.1f) {
					positions[i] = positions[i] + velocities[i];
					positions[j] = positions[j] + velocities[j];
				}
				glm::vec3 n = (positions[i] - positions[j]) / abs(positions[i] - positions[j]);
				glm::vec3 normal = ((velocities[i] - velocities[j]) * n) * n;
				velocities[i] = velocities[i] - normal;
				velocities[j] = velocities[j] + normal;
				velocities[i] = velocities[i] * (resistance * resistance);
				velocities[j] = velocities[j] * (resistance * resistance);
				positions[i] = positions[i] + (((2.0f - distance) / glm::length(velocities[i])) * velocities[i]);
				positions[j] = positions[j] + (((2.0f - distance) / glm::length(velocities[j])) * velocities[j]);
			}
		}
	}
	collisionCheck[i] = false;
}