#include "Simulation.h"
#include "imgui\imgui.h"

//The render loop
void Simulation::Render() {
	//Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.Use();
	shader.SetTexture("texImage", 0, texture);

	//Run OpenCL GPU Collision Detector
	CalculateCL();

	//The ball handler loop
	for (size_t i = 0; i < numberOfBalls; i++) {
		if (run) {
			velocities[i] = velocities[i] - gravity;
			velocities[i] = velocities[i] * resistance;
			positions[i] = positions[i] + velocities[i];
			if (ballCollisionRun)	ballCollision(i);
			wallCollision(i);
		}
		glm::mat4 suzanne1World = glm::translate(positions[i]);
		shader.SetUniform("world", suzanne1World);
		shader.SetUniform("worldIT", glm::transpose(glm::inverse(suzanne1World)));
		shader.SetUniform("MVP", camera.GetViewProj() * suzanne1World);
		shader.SetUniform("Kd", colors[i]);
		ball->draw();
	}
	//Clear the ball collision update checker
	for (size_t i = 0; i < numberOfBalls; i++) collisionCheck[i] = true;

	//Draw wall
	glDisable(GL_CULL_FACE);
	glm::mat4 wallWorld = glm::translate(glm::vec3(0, 0, 0));
	shader.SetUniform("world", wallWorld);
	shader.SetUniform("worldIT", glm::transpose(glm::inverse(wallWorld)));
	shader.SetUniform("MVP", camera.GetViewProj() * wallWorld);
	shader.SetUniform("Kd", wallColor);
	wall.Bind();
	glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_INT, 0);
	wall.Unbind();

	//Free the shader
	shader.Unuse();

	//UI Drawer
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(500, 240), ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Rigid Body Simulation"))
	{
		ImGui::Text("Rigid Body Simulation");
		ImGui::Text("By: Sandor Balazs");
		ImGui::PlotLines("FPS", fps, IM_ARRAYSIZE(fps), 0, "FPS", 0.0f, 100.0f, ImVec2(0, 80));
		ImGui::SliderFloat3("Gravity", &(gravity[0]), -0.02f, 0.02f);
		ImGui::SliderFloat("Resistance", &(resistance), 0.9f, 1.0f);
		ImGui::SliderFloat("ballInitSpeed", &(ballInitSpeed), 0.0f, 1.0f);
		if (ImGui::SliderFloat("boxSize", &(boxSize), 5.0f, 100.0f)) wallBuilder();
		ImGui::SliderInt("numberOfBalls", &(numberOfBalls), 1, numberOfBallsArray-1);
		ImGui::Checkbox("Random XZ", &randomXZ); ImGui::SameLine(150);
		ImGui::Checkbox("Random Y", &randomY);
		ImGui::Checkbox("RUN", &run); ImGui::SameLine(150);
		ImGui::Checkbox("Collision", &ballCollisionRun);
		static int clicked = 0;
		if (ImGui::Button("START")) {
			wallBuilder();
			ballInit();
		}
	}
	ImGui::End();
}