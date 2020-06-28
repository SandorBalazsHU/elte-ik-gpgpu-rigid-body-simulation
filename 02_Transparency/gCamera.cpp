#include <iostream>
#include "gCamera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>

/// <summary>
/// Initializes a new instance of the <see cref="gCamera"/> class.
/// </summary>
gCamera::gCamera(void) : m_eye(0.0f, 0.0f, 0.0f), m_at(0.0f), m_up(0.0f, 1.0f, 0.0f), m_speed(16.0f), m_goFw(0), m_goRight(0), m_eye_sphare(100.0f, 20.0f, 20.0f)
{
	SetView( glm::vec3(90,90,90), glm::vec3(0,0,0), glm::vec3(0,1,0));

	m_dist = glm::length( m_at - m_eye );	

	SetProj(45.0f, 640/480.0f, 0.001f, 1000.0f);
}

gCamera::gCamera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up) : m_speed(16.0f), m_goFw(0), m_goRight(0), m_dist(10)
{
	SetView(_eye, _at, _up);
}

gCamera::~gCamera(void)
{
}

void gCamera::SetView(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up)
{
	m_eye	= _eye;
	m_up	= _up;

	m_fw  = glm::normalize( m_at - m_eye  );
	m_st = glm::normalize( glm::cross( m_fw, m_up ) );

	m_dist = glm::length( m_at - m_eye );	

	m_u = atan2f( m_fw.z, m_fw.x );
	m_v = acosf( m_fw.y );
}

void gCamera::SetProj(float _angle, float _aspect, float _zn, float _zf)
{
	m_matProj = glm::perspective( _angle, _aspect, _zn, _zf);
	m_matViewProj = m_matProj * m_viewMatrix;
}

glm::mat4 gCamera::GetViewMatrix()
{
	return m_viewMatrix;
}

void gCamera::Update(float _deltaTime)
{
	m_eye += (m_goFw*m_fw + m_goRight*m_st)*m_speed*_deltaTime;

	m_viewMatrix = glm::lookAt( m_eye, m_at, m_up);
	m_matViewProj = m_matProj * m_viewMatrix;
}

void gCamera::SphereUpdate(float r, float a, float b)
{
	m_eye_sphare.x += r;
	m_eye_sphare.y += b*-1.0f;
	m_eye_sphare.z += a;
	m_eye.x = m_eye_sphare.x * sinf(m_eye_sphare.y) * cosf(m_eye_sphare.z);
	m_eye.z = m_eye_sphare.x * sinf(m_eye_sphare.y) * sinf(m_eye_sphare.z);
	m_eye.y = m_eye_sphare.x * cosf(m_eye_sphare.y);
}

void gCamera::SetSpeed(float _val)
{
	m_speed = _val;
}

void gCamera::Resize(int _w, int _h)
{
	m_matProj = glm::perspective(	45.0f, _w/(float)_h, 0.01f, 1000.0f);

	m_matViewProj = m_matProj * m_viewMatrix;
}

void gCamera::KeyboardDown(SDL_KeyboardEvent& key)
{
	switch ( key.keysym.sym )
	{
	case SDLK_w:
			m_goFw = 1;
		break;
	case SDLK_s:
			m_goFw = -1;
		break;
	case SDLK_a:
			m_goRight = -1;
		break;
	case SDLK_d:
			m_goRight = 1;
		break;
	}
}

void gCamera::KeyboardUp(SDL_KeyboardEvent& key)
{
	float current_speed = m_speed;
	switch ( key.keysym.sym )
	{
	case SDLK_w:
	case SDLK_s:
			m_goFw = 0;
		break;
	case SDLK_a:
	case SDLK_d:
			m_goRight = 0;
		break;
	}
}

void gCamera::MouseMove(SDL_MouseMotionEvent& mouse)
{
	if ( mouse.state & SDL_BUTTON_LMASK )
	{
		SphereUpdate(0, mouse.xrel/100.0f, mouse.yrel/100.0f);
	}
}

void gCamera::MouseWheel(SDL_MouseWheelEvent& wheel)
{
	if (wheel.y > 0) // scroll up
	{
		SphereUpdate(wheel.y, 0, 0);
	}
	else if (wheel.y < 0) // scroll down
	{
		SphereUpdate(wheel.y, 0, 0);
	}
}

void gCamera::LookAt(glm::vec3 _at)
{
	SetView(m_eye, _at, m_up);
}

