#include "gui.h"

void Gui::initialise(GLFWwindow *window)
{
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	io.FontGlobalScale = 1.5f;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}

void Gui::update(bool &useDiffuse, bool &useNormal, bool &wireFrame, float &heightScale, float &translation, float &rotation, Light &light)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Scene Control");

	// Print framerate
	ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Checkbox("Wire Frame", &wireFrame);
	ImGui::Checkbox("Use Diffuse Texture", &useDiffuse);
	ImGui::Checkbox("Use Normal Texture", &useNormal);
	ImGui::SliderFloat("Light Position", &light.position[0], -8.0f, 8.0f);
	ImGui::SliderFloat("Height Map Scale", &heightScale, 0.0f, 1.0f);
	ImGui::SliderFloat3("Model Translation", &translation, -2.0f, 2.0f);
	ImGui::SliderFloat3("Model Rotation", &rotation, -3.14f, 3.14f);

	ImGui::End();
}

void Gui::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
