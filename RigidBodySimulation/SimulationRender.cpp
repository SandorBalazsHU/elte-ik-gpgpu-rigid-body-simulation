/*
	GPU accelerated rigid body simulation with OpenGL and OpenCL.
	Created by: Sándor Balázs - it.sandor.balazs@gmail.com
	AX400
	---
	The main renderer cycle. Ball, Wall, Barrier and GUI rendering.
*/

#include "Simulation.h"
#include "imgui\imgui.h"

//The render loop
void Simulation::Render() {

	glEnable(GL_CULL_FACE); //Face test ON
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Use();
	shader.SetTexture("texImage", 0, texture);

	shader.SetUniform("eye_pos", camera.getCameraPosition());
	//shader.SetUniform("light_pos", camera.getCameraPosition());
	//shader.SetUniform("Ls", camera.getCameraPosition());

	//GPU Collision handler
	if (run && GPU_isActive) Collision_GPU();

	//The ball handler loop
	for (size_t i = 0; i < numberOfBalls; i++) {
		//CPU Collision handler
		if (run && !GPU_isActive) Collision_CPU(i);

		//Ball Drawer
		glm::mat4 wallWorld = glm::translate(positions[i]);
		shader.SetUniform("world", wallWorld);
		shader.SetUniform("worldIT", glm::transpose(glm::inverse(wallWorld)));
		shader.SetUniform("MVP", camera.getProjectionViewMatrix() * wallWorld);
		shader.SetUniform("Kd", colors[i]);
		ball->draw();

		//Debug messages
		if (debug) std::cout << i << " - " << positions[i].x << ", " << positions[i].y << ", " << positions[i].z << std::endl;
	}

	/*/Looked PointBall Drawer
	glm::mat4 ballWorld = glm::translate(camera.getLookedPoint());
	shader.SetUniform("world", ballWorld);
	shader.SetUniform("worldIT", glm::transpose(glm::inverse(ballWorld)));
	shader.SetUniform("MVP", camera.getProjectionViewMatrix() * ballWorld);
	shader.SetUniform("Kd", glm::vec4(1.0f,0.0f,0.0f,1.0f));
	ball->draw();*/

	//Clear the ball collision update checker
	for (size_t i = 0; i < numberOfBalls; i++) collisionCheck[i] = true;

	//Face test OFF
	glDisable(GL_CULL_FACE);

	//Draw barrier
	if (barrierIsOn) {
		glm::mat4 barrierWorld = glm::translate(barrierShift);
		shader.SetUniform("world", barrierWorld);
		shader.SetUniform("worldIT", glm::transpose(glm::inverse(barrierWorld)));
		shader.SetUniform("MVP", camera.getProjectionViewMatrix() * barrierWorld);
		shader.SetUniform("Kd", barrierColor);
		barrier.Bind();
		glDrawElements(GL_TRIANGLES, 32, GL_UNSIGNED_INT, 0);
		barrier.Unbind();
	}

	//Draw wall
	glm::mat4 wallWorld = glm::translate(glm::vec3(0, 0, 0));
	shader.SetUniform("world", wallWorld);
	shader.SetUniform("worldIT", glm::transpose(glm::inverse(wallWorld)));
	shader.SetUniform("MVP", camera.getProjectionViewMatrix() * wallWorld);
	shader.SetUniform("Kd", wallColor);
	wall.Bind();
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
	wall.Unbind();

	//Free the shader
	shader.Unuse();

	//UI Drawer
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(500, 240), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Rigid Body Simulation")) {
		ImGui::Text("Rigid Body Simulation");
		ImGui::Text("By: Sandor Balazs");

		ImGui::PlotLines("FPS", fps, IM_ARRAYSIZE(fps), 0, "FPS", 0.0f, 100.0f, ImVec2(0, 80));

		if (ImGui::SliderFloat3("Gravity", &(gravity[0]), -0.02f, 0.02f)) Update_GPU(false);
		if (ImGui::SliderFloat("Resistance", &(resistance), 0.9f, 1.0f)) Update_GPU(false);
		ImGui::SliderFloat("Ball Init Speed", &(ballInitSpeed), 0.0f, 1.0f);
		if (ImGui::SliderFloat("Box Size", &(boxSize), 5.0f, 100.0f)) {
			wallBuilder();
			if (barrierIsOn) barrierBuilder();
			Update_GPU(false);
		}
		if (ImGui::SliderInt("Number of Balls", &(numberOfBalls), 1, numberOfBallsArray-1)) Update_GPU(false);
		
		ImGui::Checkbox("Random XZ", &randomXZ); ImGui::SameLine(150);
		ImGui::Checkbox("Random Y", &randomY);
		ImGui::Checkbox("RUN", &run); ImGui::SameLine(150);
		if (ImGui::Checkbox("Collision", &ballCollisionRun)) Update_GPU(false);

		ImGui::Text("Collision calculation on:"); ImGui::SameLine();
		static int radioValue = 0;
		if (ImGui::RadioButton("GPU", &radioValue, 0)) if (radioValue == 0) {
			GPU_isActive = true;
			Update_GPU(true);
		} ImGui::SameLine();
		if (ImGui::RadioButton("CPU", &radioValue, 1)) if (radioValue == 1) {
			GPU_isActive = false;
			UpdateVelocitiesFrom_GPU();
		}

		if (ImGui::Button("RESET")) {
			wallBuilder();
			if(barrierIsOn) barrierBuilder();
			ballInit();
		} ImGui::SameLine(150);
		ImGui::Checkbox("Small Box", &barrierIsOn);

	}
	ImGui::End();
}