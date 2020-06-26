#pragma once

#include <SDL.h>
#include <glm/glm.hpp>

class gCamera
{
public:
	gCamera(void);
	gCamera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
	~gCamera(void);

	/// <summary>
	/// Gets the view matrix.
	/// </summary>
	/// <returns>The 4x4 view matrix</returns>
	glm::mat4 GetViewMatrix();

	void Update(float _deltaTime);

	void SetView(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up);
	void SetProj(float _angle, float _aspect, float _zn, float _zf); 
	void LookAt(glm::vec3 _at);

	void SetSpeed(float _val);
	glm::vec3 GetEye()
	{
		return m_eye;
	}

	glm::vec3 GetAt()
	{
		return m_at;
	}

	glm::vec3 GetUp()
	{
		return m_up;
	}

	glm::mat4 GetProj()
	{
		return m_matProj;
	}

	glm::mat4 GetViewProj()
	{
		return m_matViewProj;
	}

	void Resize(int _w, int _h);

	void KeyboardDown(SDL_KeyboardEvent& key);
	void KeyboardUp(SDL_KeyboardEvent& key);
	void MouseMove(SDL_MouseMotionEvent& mouse);
	void MouseWheel(SDL_MouseWheelEvent& wheel);

private:
	/// <summary>
	/// Updates the UV.
	/// </summary>
	/// <param name="du">The du, i.e. the change of spherical coordinate u.</param>
	/// <param name="dv">The dv, i.e. the change of spherical coordinate v.</param>
	void SphereUpdate(float r, float a, float b);

	/// <summary>
	///  The traversal speed of the camera
	/// </summary>
	float		m_speed;
	/// <summary>
	/// The view matrix of the camera
	/// </summary>
	glm::mat4	m_viewMatrix;

	glm::mat4	m_matViewProj;

	/// <summary>
	/// The camera position.
	/// </summary>
	glm::vec3	m_eye;

	/// <summary>
	/// The camera position in the sphere.
	/// </summary>
	glm::vec3	m_eye_sphare;

	/// <summary>
	/// The vector pointing upwards
	/// </summary>
	glm::vec3	m_up;

	/// <summary>
	/// The camera look at point.
	/// </summary>
	glm::vec3	m_at;

	/// <summary>
	/// The u spherical coordinate of the spherical coordinate pair (u,v) denoting the
	/// current viewing direction from the view position m_eye. 
	/// </summary>
	float	m_u;

	/// <summary>
	/// The v spherical coordinate of the spherical coordinate pair (u,v) denoting the
	/// current viewing direction from the view position m_eye. 
	/// </summary>
	float	m_v;

	/// <summary>
	/// The distance of the look at point from the camera. 
	/// </summary>
	float	m_dist;

	/// <summary>
	/// The unit vector pointing towards the viewing direction.
	/// </summary>
	glm::vec3	m_fw;
	/// <summary>
	/// The unit vector pointing to the 'right'
	/// </summary>
	glm::vec3	m_st;

	glm::mat4	m_matProj;

	float	m_goFw;
	float	m_goRight;
};

