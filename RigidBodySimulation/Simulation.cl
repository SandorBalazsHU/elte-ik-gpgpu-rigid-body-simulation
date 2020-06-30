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

	float3 iVelocity = vload3(i, velocities);
	float3 iPosition = vload3(i, positions);

	//Apply the velocities, the gravity and the resistance
	iVelocity = iVelocity - gravity;
	iVelocity = iVelocity * resistance;
	iPosition = iPosition + iVelocity;

	//Handle the ball to ball collision and reflection
	if (ballCollisionRun == 1) {
		for (size_t j = 0; j < numberOfBalls; j++) {
			if (i != j) {
				float3 jVelocity = vload3(j, velocities);
				float3 jPosition = vload3(j, positions);
				float dist = fast_distance(iPosition, jPosition);
				if (dist < 2.0f) {
					//Becouse zero division.
					if (dist <= 0.1f) {
						iPosition = iPosition + iVelocity;
						jPosition = jPosition + jVelocity;
					}
					float iLength = fast_length(iVelocity);
					float3 p = iPosition - jPosition;
					float3 v = iVelocity - jVelocity;
					iVelocity = normalize(iVelocity - (dot(v, p)) / (sqrt(v.x * v.x + v.y * v.y + v.z * v.z)) * p) * (iLength/2.0f);
					float jLength = fast_length(jVelocity);
					p = jPosition - iPosition;
					v = jVelocity - iVelocity;
					jVelocity = normalize(jVelocity - (dot(v, p)) / (sqrt(v.x * v.x + v.y * v.y + v.z * v.z)) * p) * (jLength / 2.0f);

					iVelocity = iVelocity * (resistance * resistance); //Plus resistance
					jVelocity = jVelocity * (resistance * resistance); //Plus resistance
					iPosition = iPosition + (((2.2f - dist) / length(iVelocity)) * iVelocity);
					jPosition = jPosition - (((2.2f - dist) / length(jVelocity)) * jVelocity);
				
					vstore3(jVelocity, j, velocities);
					vstore3(jPosition, j, positions);
				}
			}
		}
	}

	//Handle the ball to wall collision and reflection
	float3 normal = (float3)(0.0f, 0.0f, 0.0f);

	if (iPosition.y <= -boxSize + 1.0f) {
		normal = (float3)(0.0f, 1.0f, 0.0f);
		iPosition.y = -boxSize + 1.1f;
	}
	if (iPosition.y >= boxSize - 1.0f) {
		normal = (float3)(0.0f, -1.0f, 0.0f);
		iPosition.y = boxSize - 1.1f;
	}
	if (iPosition.x <= -boxSize + 1.0f) {
		normal = (float3)(1.0f, 0.0f, 0.0f);
		iPosition.x = -boxSize + 1.1f;
	}
	if (iPosition.x >= boxSize - 1.0f) {
		normal = (float3)(-1.0f, 0.0f, 0.0f);
		iPosition.x = boxSize - 1.1f;
	}
	if (iPosition.z <= -boxSize + 1.0f) {
		normal = (float3)(0.0f, 0.0f, 1.0f);
		iPosition.z = -boxSize + 1.1f;
	}
	if (iPosition.z >= boxSize - 1.0f) {
		normal = (float3)(0.0f, 0.0f, -1.0f);
		iPosition.z = boxSize - 1.1f;
	}

	if (normal.x != 0.0f || normal.y != 0.0f || normal.z != 0.0f) {
		iVelocity = iVelocity - 2.0f * dot(normal, iVelocity) * normal;
		iVelocity = iVelocity * resistance; //Plus resistance
	}

	vstore3(iVelocity, i, velocities);
	vstore3(iPosition, i, positions);
}