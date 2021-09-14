/**
 * @name Traffic Simulation.
 * @file Camera.cpp
 * @class Camera
 * @author Sándor Balázs - AZA6NL
 * @date 2021.05.02.
 * @brief Camera implementation file with spherical coordinates.
 * Contact: sandorbalazs9402@gmail.com
*/

#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <set>
#include <iostream>

/**
 * @brief The Camera class constructor.
 * Initialize the variables, set the view and the projection matrix.
*/
Camera::Camera(void) : cameraPosition(0.0f), lookedPoint(0.0f), upwardDirection(defaultUpwardVector), sphericalCameraPosition(0.0f) {
	setView(defaultCameraPosition, defaultLookedPoint);
	setProjection(angle, aspect, zNear, zFar);
}

/**
 * @brief The Camera class destructor.
 * Don't have function at the moment.
*/
Camera::~Camera(void) {}

/**
 * @brief Set the camera position and the looked point.
 * Generate the spherical coordinates from the sphere position and update the camera position.
 * @param cameraPosition The settable camera position.
 * @param lookedPoint The settable looked point.
*/
void Camera::setView(glm::vec3 camPos, glm::vec3 lookPoint) {
	cameraPosition = camPos;
	lookedPoint = lookPoint;
	sphericalCoordinateUpdate();
	cameraCoordinateUpdate();
}

/**
 * @brief Adapter function for the SetView.
 * @param cameraPosition The settable camera position.
*/
void Camera::setCameraPosition(glm::vec3 camPos) {
	setView(camPos, lookedPoint);
}

/**
 * @brief Adapter function for the SetView.
 * @param lookedPoint The settable looked point.
*/
void Camera::setLookedPoint(glm::vec3 lookPoint) {
	setView(cameraPosition, lookPoint);
}

/**
 * @brief Update viewMatrix from cameraPosition, lookedPoint and upwardDirection for apply changes.
*/
void Camera::update() {
	viewMatrix = glm::lookAt(cameraPosition, lookedPoint, upwardDirection);
}

/**
 * \attention The camera z axis show upward! \n
 * @brief Calculate camera coordinates from current spherical coordinates. \n
 * \f$ sphericalCameraPosition.x = radius = r \f$ \n
 * \f$ sphericalCameraPosition.y = inclination = \theta \f$ \n
 * \f$ sphericalCameraPosition.z = azimuth = \varphi \f$ \n
 * \f$ cameraPosition.x = r\cos{\varphi}\sin{\theta}+lookedPoint.x \f$ \n
 * \f$ cameraPosition.z = r\sin{\varphi}\sin{\theta}+ lookedPoint.z \f$ \n
 * \f$ cameraPosition.y = r\cos{\theta}+lookedPoint.y \f$ \n
*/
void Camera::cameraCoordinateUpdate() {
	cameraPosition.x = (sphericalCameraPosition.x * sinf(sphericalCameraPosition.y) * cosf(sphericalCameraPosition.z)) + lookedPoint.x;
	cameraPosition.z = (sphericalCameraPosition.x * sinf(sphericalCameraPosition.y) * sinf(sphericalCameraPosition.z)) + lookedPoint.z;
	cameraPosition.y = (sphericalCameraPosition.x * cosf(sphericalCameraPosition.y)) + lookedPoint.y;
	update();
}

/**
 * \attention The camera z axis show upward! \n
 * In this point we use the origo shifted camera coordinates \f$ origoCameraPosition = cameraPosition - lookedPoint \f$ \n
 * Built in zero division prevention. \n
 * @brief Calculate spherical coordinates from current camera coordinates. \n
 * \f$ sphericalCameraPosition.x = radius = r = \sqrt{x^{2}+y^{2}+z^{2}} \f$ \n
 * \f$ sphericalCameraPosition.y = inclination = \theta = \arccos{\frac{y}{r}} \f$ \n
 * \f$ sphericalCameraPosition.z = azimuth = \varphi = \arctan{\frac{z}{x}} \f$ \n
*/
void Camera::sphericalCoordinateUpdate() {
	glm::vec3 origoCameraPosition = cameraPosition - lookedPoint;
	origoCameraPosition.x = (origoCameraPosition.x == 0.0f) ? 0.0001f : origoCameraPosition.x;
	sphericalCameraPosition.x = sqrt((origoCameraPosition.x * origoCameraPosition.x) + (origoCameraPosition.y * origoCameraPosition.y) + (origoCameraPosition.z * origoCameraPosition.z));
	sphericalCameraPosition.x = (sphericalCameraPosition.x == 0.0f) ? 0.0001f : sphericalCameraPosition.x;
	sphericalCameraPosition.y = acosf(origoCameraPosition.y / sphericalCameraPosition.x);
	sphericalCameraPosition.z = atanf(origoCameraPosition.z / origoCameraPosition.x);
}

/**
 * @brief Shift the spherical coordinates. (with limitations)
 * @param radius The spherical coordinates radius;
 * @param azimuth The spherical coordinates azimuth;
 * @param inclination The spherical coordinates inclination;
*/
void Camera::sphericalCoordinateShift(float radius, float azimuth, float inclination) {
	sphericalCameraPosition.x += radius;
	sphericalCameraPosition.y += inclination;
	sphericalCameraPosition.z += azimuth;
	//Limiters
	if (sphericalCameraPosition.x < 1.0f) sphericalCameraPosition.x = 1.2f;
	if (sphericalCameraPosition.y > 1.5f) sphericalCameraPosition.y = 1.499f;
	if (sphericalCameraPosition.y < 0.02f) sphericalCameraPosition.y = 0.0211f;
	cameraCoordinateUpdate();
}

/**
 * @brief Calculate the projection matrix for the camera transformation.
 * @param angle The view pyramid angle.
 * @param aspect Te view aspect.
 * @param zNear Near cutting plane.
 * @param zFar Far cutting plane.
*/
void Camera::setProjection(float angle, float aspect, float zNear, float zFar) {
	projectionMatrix = glm::perspective(angle, aspect, zNear, zFar);
}

/**
 * @brief Camera projectionMatrix update function for the screen resize.
 * @param width The OpenGL screen width.
 * @param height The OpenGL screen height.
*/
void Camera::resize(int width, int height) {
	aspect = ((float)width) / ((float)height);
	setProjection(angle, aspect, zNear, zFar);
}

/**
 * @brief Event handler function for the key down event.
 * Store the pressed keys in the pressedKeys set.
 * @param key keyboard down event.
*/
void Camera::keyboardDown(SDL_KeyboardEvent& key) {
	float s = 0.5f;
	pressedKeys.insert(key.keysym.sym);
	for (int keyCode : pressedKeys) {
		switch (keyCode) {
		case SDLK_w:
			lookedPoint.x += s;
			cameraPosition.x += s;
			break;
		case SDLK_s:
			lookedPoint.x -= s;
			cameraPosition.x -= s;
			break;
		case SDLK_a:
			lookedPoint.z += s;
			cameraPosition.z += s;
			break;
		case SDLK_d:
			lookedPoint.z -= s;
			cameraPosition.z -= s;
			break;
		}
	}
	sphericalCoordinateUpdate();
	cameraCoordinateUpdate();
}

/**
 * @brief Event handler function for the key up event.
 * Erase the key from the pressedKeys set.
 * @param key keyboard up event.
*/
void Camera::keyboardUp(SDL_KeyboardEvent& key) {
	pressedKeys.erase(key.keysym.sym);
}

/**
* @brief Event handler function for the mouse move event.
* @param key mouse move event.
*/
void Camera::mouseMove(SDL_MouseMotionEvent& mouse) {
	if (mouse.state & SDL_BUTTON_LMASK) {
		sphericalCoordinateShift(0, mouse.xrel / 200.0f, (mouse.yrel / 200.0f) * -1.0f);
	}
}

/**
* @brief Event handler function for the mouse wheel event.
* @param key mouse wheel event.
*/
void Camera::mouseWheel(SDL_MouseWheelEvent& wheel) {
	if (wheel.y > 0) {
		sphericalCoordinateShift(wheel.y * 1.5f, 0, 0);
	}
	else if (wheel.y < 0) {
		sphericalCoordinateShift(wheel.y * 1.5f, 0, 0);
	}
}