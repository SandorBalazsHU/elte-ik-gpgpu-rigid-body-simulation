#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(void) : m_eye(0.0f, 0.0f, 0.0f), m_at(0.0f), m_up(0.0f, 1.0f, 0.0f), m_goFw(0), m_goRight(0), m_eye_sphare(77.94228634f, 0.78539816f, 0.95531661f) {
	SetView( glm::vec3(45,45,45), glm::vec3(0,0,0), glm::vec3(0,1,0));
	m_dist = glm::length( m_at - m_eye );	
	SetProj(45.0f, 640/480.0f, 0.001f, 1000.0f);
}

Camera::Camera(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up) : m_goFw(0), m_goRight(0), m_dist(10) {
	SetView(_eye, _at, _up);
}

Camera::~Camera(void) {}

void Camera::SetView(glm::vec3 _eye, glm::vec3 _at, glm::vec3 _up) {
	m_eye	= _eye;
	m_up	= _up;

	m_fw  = glm::normalize( m_at - m_eye  );
	m_st = glm::normalize( glm::cross( m_fw, m_up ) );

	m_dist = glm::length( m_at - m_eye );	

	m_u = atan2f( m_fw.z, m_fw.x );
	m_v = acosf( m_fw.y );
}

void Camera::SetProj(float _angle, float _aspect, float _zn, float _zf) {
	m_matProj = glm::perspective( _angle, _aspect, _zn, _zf);
	m_matViewProj = m_matProj * m_viewMatrix;
}

glm::mat4 Camera::GetViewMatrix() {
	return m_viewMatrix;
}

void Camera::Update() {
	m_eye += (m_goFw*m_fw + m_goRight*m_st);
	m_viewMatrix = glm::lookAt( m_eye, m_at, m_up);
	m_matViewProj = m_matProj * m_viewMatrix;
}

void Camera::SphereUpdate(float r, float a, float b) {
	m_eye_sphare.x += r;
	m_eye_sphare.y += b*-1.0f;
	m_eye_sphare.z += a;
	m_eye.x = m_eye_sphare.x * sinf(m_eye_sphare.y) * cosf(m_eye_sphare.z);
	m_eye.z = m_eye_sphare.x * sinf(m_eye_sphare.y) * sinf(m_eye_sphare.z);
	m_eye.y = m_eye_sphare.x * cosf(m_eye_sphare.y);
}

void Camera::Resize(int _w, int _h) {
	m_matProj = glm::perspective(	45.0f, _w/(float)_h, 0.01f, 1000.0f);
	m_matViewProj = m_matProj * m_viewMatrix;
}

void Camera::KeyboardDown(SDL_KeyboardEvent& key){}
void Camera::KeyboardUp(SDL_KeyboardEvent& key){}

void Camera::MouseMove(SDL_MouseMotionEvent& mouse) {
	if ( mouse.state & SDL_BUTTON_LMASK ) {
		SphereUpdate(0, mouse.xrel/100.0f, mouse.yrel/100.0f);
	}
}

void Camera::MouseWheel(SDL_MouseWheelEvent& wheel) {
	// scroll up
	if (wheel.y > 0) {
		SphereUpdate(wheel.y*2.0f, 0, 0);
	}
	// scroll down
	else if (wheel.y < 0) {
		SphereUpdate(wheel.y*2.0f, 0, 0);
	}
}

void Camera::LookAt(glm::vec3 _at) {
	SetView(m_eye, _at, m_up);
}

