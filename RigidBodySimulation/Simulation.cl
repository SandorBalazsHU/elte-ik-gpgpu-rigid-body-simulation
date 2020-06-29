__kernel void update(
	__global float3* positions,
	__global float3* velocities,
	__global bool* collisionCheck,
	int numberOfBalls,
	float boxSize,
	float resistance,
	float3 gravity)
{
	int i = get_global_id(0);

	velocities[i] = velocities[i] - gravity;
	velocities[i] = velocities[i] * resistance;
	positions[i] = positions[i] + velocities[i];

	float3 normal = (float3)(0.0f, 0.0f, 0.0f);

	if (positions[i].y <= -boxSize + 1.0f) {
		normal = (float3)(0.0f, 1.0f, 0.0f);
		positions[i].y = -boxSize + 1.1f;
	}
	if (positions[i].y >= boxSize - 1.0f) {
		normal = (float3)(0.0f, -1.0f, 0.0f);
		positions[i].y = boxSize - 1.1f;
	}
	if (positions[i].x <= -boxSize + 1.0f) {
		normal = (float3)(1.0f, 0.0f, 0.0f);
		positions[i].x = -boxSize + 1.1f;
	}
	if (positions[i].x >= boxSize - 1.0f) {
		normal = (float3)(-1.0f, 0.0f, 0.0f);
		positions[i].x = boxSize - 1.1f;
	}
	if (positions[i].z <= -boxSize + 1.0f) {
		normal = (float3)(0.0f, 0.0f, 1.0f);
		positions[i].z = -boxSize + 1.1f;
	}
	if (positions[i].z >= boxSize - 1.0f) {
		normal = (float3)(0.0f, 0.0f, -1.0f);
		positions[i].z = boxSize - 1.1f;
	}

	if (normal.x != 0.0f || normal.y != 0.0f || normal.z != 0.0f) {
		float3 dot = (float3)(normal.x * velocities[i].x, normal.y * velocities[i].y, normal.z * velocities[i].z);
		velocities[i] = velocities[i] - 2.0f * dot * normal;
		velocities[i] = velocities[i] * resistance; //Plus resistance
	}

	/*//Handle the ball to ball collision and reflection
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
					//std::cout << "COLLISION " << i << "," << j << std::endl;
					glm::vec3 n = (positions[i] - positions[j]) / abs(positions[i] - positions[j]);
					glm::vec3 normal = ((velocities[i] - velocities[j]) * n) * n;
					velocities[i] = velocities[i] - normal;
					velocities[j] = velocities[j] + normal;
					velocities[i] = velocities[i] * (resistance * resistance); //Plus resistance
					velocities[j] = velocities[j] * (resistance * resistance); //Plus resistance
					positions[i] = positions[i] + (((2.0f - distance) / glm::length(velocities[i])) * velocities[i]);
					positions[j] = positions[j] + (((2.0f - distance) / glm::length(velocities[j])) * velocities[j]);
					//if (glm::length(velocities[i]) <= 0.1f) velocities[i] = glm::vec3(0);
					//if (glm::length(velocities[j]) <= 0.1f) velocities[j] = glm::vec3(0);
				}
			}
		}
		collisionCheck[i] = false;
	}*/
}