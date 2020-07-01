#include "Simulation.h"
#include "imgui\imgui.h"

//The render loop
void Simulation::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	shader.Use();
	shader.SetTexture("texImage", 0, texture);

	//GPU Collision handler
	if (run && GPU_isActive) Collision_GPU();

	//The ball handler loop
	for (size_t i = 0; i < numberOfBalls; i++) {
		//CPU Collision handler
		if (run && !GPU_isActive) Collision_CPU(i);
		//Ball Drawer
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
	if (ImGui::Begin("Rigid Body Simulation")) {
		ImGui::Text("Rigid Body Simulation");
		ImGui::Text("By: Sandor Balazs");

		ImGui::PlotLines("FPS", fps, IM_ARRAYSIZE(fps), 0, "FPS", 0.0f, 100.0f, ImVec2(0, 80));

		if (ImGui::SliderFloat3("Gravity", &(gravity[0]), -0.02f, 0.02f)) Update_GPU(false);
		if (ImGui::SliderFloat("Resistance", &(resistance), 0.9f, 1.0f)) Update_GPU(false);
		ImGui::SliderFloat("Ball Init Speed", &(ballInitSpeed), 0.0f, 1.0f);
		if (ImGui::SliderFloat("Box Size", &(boxSize), 5.0f, 100.0f)) {
			wallBuilder();
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
			ballInit();
		}
	}
	ImGui::End();
}