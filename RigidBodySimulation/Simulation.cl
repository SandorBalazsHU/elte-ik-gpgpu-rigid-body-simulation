__kernel void update(
	__global float* positions,
	__global float* velocities,
	int numberOfBalls,
	float boxSize,
	float resistance,
	float3 gravity,
	int ballCollisionRun)
{
	//The main counter
	int i = get_global_id(0);

	float3 currentVelocity = vload3(i, velocities);
	float3 currentPosition = vload3(i, positions);

	//Apply the velocities, the gravity and the resistance
	currentVelocity = currentVelocity - gravity;
	currentVelocity = currentVelocity * resistance;
	currentPosition  = currentPosition  + currentVelocity;

	//Handle the ball to ball collision and reflection
	/*if (ballCollisionRun == 1) {
		for (size_t j = 0; j < numberOfBalls; j++) {
			if (i != j) {
				float dist = distance(positions[i], positions[j]);
				if (dist < 2.0f) {
					//Becouse zero division.
					if (dist <= 0.1f) {
						positions[i] = positions[i] + velocities[i];
						positions[j] = positions[j] + velocities[j];
					}
					float3 p = positions[i] - positions[j];
					float3 v = velocities[i] - velocities[j];
					velocities[i] = velocities[i] - normalize((dot(v, p)) / (sqrt(v.x * v.x + v.y * v.y + v.z * v.z))) * p;

					//velocities[i] = velocities[i] * (resistance * resistance); //Plus resistance
					//velocities[j] = velocities[j] * (resistance * resistance); //Plus resistance
					//positions[i] = positions[i] + ((((2.1f - dist)/2.0f) / length(velocities[i])) * velocities[i]);
					//positions[j] = positions[j] + (((2.0f - dist) / length(velocities[j])) * velocities[j]);
				}
			}
		}
	}*/

	//Handle the ball to wall collision and reflection
	float3 normal = (float3)(0.0f, 0.0f, 0.0f);

	if (currentPosition.y <= -boxSize + 1.0f) {
		normal = (float3)(0.0f, 1.0f, 0.0f);
		currentPosition.y = -boxSize + 1.1f;
	}
	if (currentPosition.y >= boxSize - 1.0f) {
		normal = (float3)(0.0f, -1.0f, 0.0f);
		currentPosition.y = boxSize - 1.1f;
	}
	if (currentPosition.x <= -boxSize + 1.0f) {
		normal = (float3)(1.0f, 0.0f, 0.0f);
		currentPosition.x = -boxSize + 1.1f;
	}
	if (currentPosition.x >= boxSize - 1.0f) {
		normal = (float3)(-1.0f, 0.0f, 0.0f);
		currentPosition.x = boxSize - 1.1f;
	}
	if (currentPosition.z <= -boxSize + 1.0f) {
		normal = (float3)(0.0f, 0.0f, 1.0f);
		currentPosition.z = -boxSize + 1.1f;
	}
	if (currentPosition.z >= boxSize - 1.0f) {
		normal = (float3)(0.0f, 0.0f, -1.0f);
		currentPosition.z = boxSize - 1.1f;
	}

	if (normal.x != 0.0f || normal.y != 0.0f || normal.z != 0.0f) {
		currentVelocity = currentVelocity - 2.0f * dot(normal, currentVelocity) * normal;
		currentVelocity = currentVelocity * resistance; //Plus resistance
	}

	vstore3(currentVelocity, i, velocities);
	vstore3(currentPosition, i, positions);
}