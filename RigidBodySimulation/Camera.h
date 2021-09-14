/**
 * @name Traffic Simulation.
 * @file Camera.h
 * @class Camera
 * @author Sándor Balázs - AZA6NL
 * @date 2021.05.01.
 * @brief Camera implementation header with spherical coordinates.
 * Contact sandorbalazs9402@gmail.com
*/

#pragma once
#include <SDL.h>
#include <glm/glm.hpp>
#include <set>

/**
 * @brief The own OpenGL camera class.
 * The camera movable in a sphere with the mouse and in the scene with the keyboard.
*/
class Camera {
public:
	/**
	 * @brief The Camera class constructor.
	 * Initialize the variables, set the view and the projection matrix.
	*/
	Camera(void);

	/**
	 * @brief The Camera class destructor.
	 * Don't have function at the moment.
	*/
	~Camera(void);

	/**
	 * @brief Set the camera position and the looked point.
	 * Generate the spherical coordinates from the sphere position and update the camera position.
	 * @param cameraPosition The settable camera position.
	 * @param lookedPoint The settable looked point.
	*/
	void setView(glm::vec3 cameraPosition, glm::vec3 lookedPoint);

	/**
	 * @brief Adapter function for the SetView.
	 * @param cameraPosition The settable camera position.
	*/
	void setCameraPosition(glm::vec3 cameraPosition);

	/**
	 * @brief Adapter function for the SetView.
	 * @param lookedPoint The settable looked point.
	*/
	void setLookedPoint(glm::vec3 lookedPoint);

	/**
	 * @brief Getter for the camera position.
	 * @return The current camera position
	*/
	glm::vec3 getCameraPosition() {
		return cameraPosition;
	}

	/**
	 * @brief Getter for the cameras looked point.
	 * @return The current looked point.
	*/
	glm::vec3 getLookedPoint() {
		return lookedPoint;
	}

	/**
	 * @brief Getter for the upward direction vector.
	 * @return The current upward vector.
	*/
	glm::vec3 getUpwardVector() {
		return upwardDirection;
	}

	/**
	 * @brief Getter for the view matrix.
	 * @return The current view matrix.
	*/
	glm::mat4 getViewMatrix() {
		return viewMatrix;
	}

	/**
	 * @brief Getter for the projection matrix.
	 * @return The current projection matrix.
	*/
	glm::mat4 getProjectionMatrix() {
		return projectionMatrix;
	}

	/**
	 * @brief The View * Projection matrix used for the camera transformation in the render.
	 * @return The product of the view and the projection matrix.
	*/
	glm::mat4 getProjectionViewMatrix() {
		return projectionMatrix * viewMatrix;
	}

	/**
	 * @brief Camera projectionMatrix update function for the screen resize.
	 * @param width The OpenGL screen width.
	 * @param height The OpenGL screen height.
	*/
	void resize(int width, int height);

	/**
	 * @brief Event handler function for the key down event.
	 * Store the pressed keys in the pressedKeys set.
	 * @param key keyboard down event.
	*/
	void keyboardDown(SDL_KeyboardEvent& key);

	/**
	 * @brief Event handler function for the key up event.
	 * Erase the key from the pressedKeys set.
	 * @param key keyboard up event.
	*/
	void keyboardUp(SDL_KeyboardEvent& key);

	/**
	* @brief Event handler function for the mouse move event.
	* @param key mouse move event.
	*/
	void mouseMove(SDL_MouseMotionEvent& mouse);

	/**
	* @brief Event handler function for the mouse wheel event.
	* @param key mouse wheel event.
	*/
	void mouseWheel(SDL_MouseWheelEvent& wheel);

protected:
	/**
	 * @brief The start camera position.
	*/
	const glm::vec3 defaultCameraPosition = glm::vec3(45.0f, 45.0f, 45.0f);

	/**
	 * @brief The start looked point.
	*/
	const glm::vec3 defaultLookedPoint = glm::vec3(0.0f);

	/**
	 * @brief The default upward vector.
	*/
	const glm::vec3 defaultUpwardVector = glm::vec3(0.0f, 10.0f, 0.0f);

	/**
	 * @brief The default angle.
	*/
	const float angle = 45.0f;

	/**
	* @brief The default angle.
	*/
	float aspect = 640 / 480.0f;

	/**
	* @brief The default angle.
	*/
	const float zNear = 0.001f;

	/**
	* @brief The default angle.
	*/
	const float zFar = 1000.0f;

	/**
	 * @brief Update viewMatrix from cameraPosition, lookedPoint and upwardDirection for apply changes.
	*/
	void update();

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
	void cameraCoordinateUpdate();

	/**
	 * \attention The camera z axis show upward! \n
	 * In this point we use the origo shifted camera coordinates \f$ origoCameraPosition = cameraPosition - lookedPoint \f$ \n
	 * Built in zero division prevention. \n
	 * @brief Calculate spherical coordinates from current camera coordinates. \n
	 * \f$ sphericalCameraPosition.x = radius = r = \sqrt{x^{2}+y^{2}+z^{2}} \f$ \n
	 * \f$ sphericalCameraPosition.y = inclination = \theta = \arccos{\frac{y}{r}} \f$ \n
	 * \f$ sphericalCameraPosition.z = azimuth = \varphi = \arctan{\frac{z}{x}} \f$ \n
	*/
	void sphericalCoordinateUpdate();

	/**
	 * @brief Shift the spherical coordinates. (with limitations)
	 * @param radius The spherical coordinates radius;
	 * @param azimuth The spherical coordinates azimuth;
	 * @param inclination The spherical coordinates inclination;
	*/
	void sphericalCoordinateShift(float radius, float azimuth, float inclination);

	/**
	 * @brief Calculate the projection matrix for the camera transformation.
	 * @param angle The view pyramid angle.
	 * @param aspect Te view aspect.
	 * @param zNear Near cutting plane.
	 * @param zFar Far cutting plane.
	*/
	void setProjection(float angle, float aspect, float zNear, float zFar);

	/**
	* @brief The camera position in spherical coordinate.
	* \f$ sphericalCameraPosition.x = radius = r=\sqrt{x^{2}+y^{2}+z^{2}} \f$
	* \f$ sphericalCameraPosition.y = inclination = \theta=\arccos{\frac{z}{r}} \f$
	* \f$ sphericalCameraPosition.z = azimuth = \varphi=\arctan{\frac{y}{x}} \f$
	*/
	glm::vec3	sphericalCameraPosition;

	/**
	* @brief The view matrix of the camera.
	*/
	glm::mat4	viewMatrix;

	/**
	* @brief The camera position.
	*/
	glm::vec3	cameraPosition;


	/**
	* The vector pointing upwards.
	*/
	glm::vec3	upwardDirection;

	/**
	* The camera look at point.
	*/
	glm::vec3	lookedPoint;

	/**
	* The projection matrix.
	*/
	glm::mat4	projectionMatrix;

	/**
	 * @brief This set store the current pressed keys.
	*/
	std::set<int> pressedKeys;
};