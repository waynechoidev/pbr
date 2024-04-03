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

void Gui::update(float &heightScale, bool &useDirectLight, bool &useEnvLight, float &lightPos, float *viewRotation)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Scene Control");

	ImGui::Text("View");
	ImGui::SliderFloat2("View rotation", viewRotation, -3.14f, 3.14f);

	// Print framerate
	ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::SliderFloat("Height Map Scale", &heightScale, 0.0f, 1.0f);
	ImGui::Checkbox("Use Direct Light", &useDirectLight);
	ImGui::Checkbox("Use Environment Light", &useEnvLight);
	ImGui::SliderFloat3("Light Position", &lightPos, -5.0f, 5.0f);
	ImGui::End();
}

void Gui::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
