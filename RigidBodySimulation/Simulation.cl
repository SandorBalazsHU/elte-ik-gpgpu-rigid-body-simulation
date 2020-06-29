__kernel void update(
	__global float3* positions,
	__global float3* velocities,
	__global bool* collisionCheck,
	int numberOfBalls,
	float boxSize)
{
	/*// Numerical integration for speed and distance
	//   v = a * t
	//   s = v * t

	// Relate force, mass and acceleration
	//   F = m*a

	// Newton: Law of universal gravitation
	//   Givew two object with mass m_1, m_2 and distance d
	//   F_grav = (G * m_1 * m_2) / d^2
	//
	// (Tip: add a small constant to divisor to avoid numerical errors!)

	const float G = 0.0001; // gravitational constant

	int gid = get_global_id(0);
	float2 p_gid = p[gid];
	float m_gid = m[gid];

	// Aggregate acceleration for each object
	float2 a_gid = { 0.0f, 0.0f };
	for (int i = 0; i < get_global_size(0); ++i)
	{
		if (i == gid) continue;
		float2 p_other = p[i];
		float m_other = m[i];

		// 1. compute force and distance
		// 2. compute acceleration
		// 3. aggregate acceleration
	}

	// 4. integrate speed (using dt and a_gid)
	//v[gid] = 

	// 5. integrate position
	//p[gid] = */
}