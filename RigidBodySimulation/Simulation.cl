/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
	---
	The GPU collision and reflection implemenatation.
	Implement the ball to wall, ball to barrier and ball to ball collision.
*/

__kernel void update( __global float* positions, __global float* velocities, int numberOfBalls,
					  float boxSize, float resistance, float3 gravity, int ballCollisionRun,
					  float3 barrierShift, int barrierIsOn) 
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
						iPosition = iPosition + iVelocity + 2.0f;
						jPosition = jPosition + jVelocity + 2.0f;
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
					iPosition = iPosition + (((2.2f - dist) / fast_length(iVelocity)) * iVelocity);
					jPosition = jPosition - (((2.2f - dist) / fast_length(jVelocity)) * jVelocity);
				
					vstore3(jVelocity, j, velocities);
					vstore3(jPosition, j, positions);
				}
			}
		}
	}

	//Handle the ball to marrier collision and refraction
	if (barrierIsOn == 1) {
		float3 normal2 = (float3)(0.0f, 0.0f, 0.0f);
		float barrierSize = boxSize / 6;

		if (iPosition.y <= (-boxSize + barrierSize * 2.0f) + 1.0f &&
			(iPosition.x >= -barrierSize + barrierShift.x) && (iPosition.x <= barrierSize + barrierShift.x) &&
			(iPosition.z >= -barrierSize + barrierShift.z) && (iPosition.z <= barrierSize + +barrierShift.z)) {
			normal2 = (float3)(0.0f, 1.0f, 0.0f);
			iPosition.y = (-boxSize + barrierSize * 2.0f) + 1.01f;
		}
		if (iPosition.y < (-boxSize + barrierSize * 2.0f) &&
			(iPosition.x > -barrierSize + barrierShift.x - 1.0f) && (iPosition.x < -barrierSize + barrierShift.x) &&
			(iPosition.z > -barrierSize + barrierShift.z) && (iPosition.z < barrierSize + +barrierShift.z)) {
			normal2 = (float3)(1.0f, 0.0f, 0.0f);
			iPosition.x = -barrierSize + barrierShift.x - 1.01f;
		}
		if (iPosition.y < (-boxSize + barrierSize * 2.0f) &&
			(iPosition.x < barrierSize + barrierShift.x + 1.0f) && (iPosition.x > barrierSize + barrierShift.x) &&
			(iPosition.z > -barrierSize + barrierShift.z) && (iPosition.z < barrierSize + +barrierShift.z)) {
			normal2 = (float3)(-1.0f, 0.0f, 0.0f);
			iPosition.x = barrierSize + barrierShift.x + 1.02f;
		}

		if (iPosition.y < (-boxSize + barrierSize * 2.0f) &&
			(iPosition.z > -barrierSize + barrierShift.z - 1.0f) && (iPosition.z < -barrierSize + barrierShift.z) &&
			(iPosition.x > -barrierSize + barrierShift.x) && (iPosition.x < barrierSize + +barrierShift.x)) {
			normal2 = (float3)(0.0f, 0.0f, 1.0f);
			iPosition.z = -barrierSize + barrierShift.z - 1.01f;
		}
		if (iPosition.y < (-boxSize + barrierSize * 2.0f) &&
			(iPosition.z < barrierSize + barrierShift.z + 1.0f) && (iPosition.z > barrierSize + barrierShift.z) &&
			(iPosition.x > -barrierSize + barrierShift.x) && (iPosition.x < barrierSize + +barrierShift.x)) {
			normal2 = (float3)(0.0f, 0.0f, -1.0f);
			iPosition.z = barrierSize + barrierShift.z + 1.02f;
		}

		if (normal2.x != 0.0f || normal2.y != 0.0f || normal2.z != 0.0f) {
			iVelocity = iVelocity - 2.0f * dot(normal2, iVelocity) * normal2;
			iVelocity = ((iVelocity * resistance) * resistance) * resistance; //Plus resistance
		}
	}

	//Handle the ball to wall collision and reflection
	float3 normal = (float3)(0.0f, 0.0f, 0.0f);

	if (iPosition.y <= -boxSize + 1.0f) {
		normal = (float3)(0.0f, 1.0f, 0.0f);
		iPosition.y = -boxSize + 1.01f;
	}
	if (iPosition.y >= boxSize - 1.0f) {
		normal = (float3)(0.0f, -1.0f, 0.0f);
		iPosition.y = boxSize - 1.01f;
	}
	if (iPosition.x <= -boxSize + 1.0f) {
		normal = (float3)(1.0f, 0.0f, 0.0f);
		iPosition.x = -boxSize + 1.01f;
	}
	if (iPosition.x >= boxSize - 1.0f) {
		normal = (float3)(-1.0f, 0.0f, 0.0f);
		iPosition.x = boxSize - 1.01f;
	}
	if (iPosition.z <= -boxSize + 1.0f) {
		normal = (float3)(0.0f, 0.0f, 1.0f);
		iPosition.z = -boxSize + 1.01f;
	}
	if (iPosition.z >= boxSize - 1.0f) {
		normal = (float3)(0.0f, 0.0f, -1.0f);
		iPosition.z = boxSize - 1.01f;
	}

	if (normal.x != 0.0f || normal.y != 0.0f || normal.z != 0.0f) {
		iVelocity = iVelocity - 2.0f * dot(normal, iVelocity) * normal;
		iVelocity = ((iVelocity * resistance)* resistance)* resistance; //Plus resistance
	}

	vstore3(iVelocity, i, velocities);
	vstore3(iPosition, i, positions);
}