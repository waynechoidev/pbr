#pragma once

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

class Gui
{
public:
	Gui(){};

	void initialise(GLFWwindow *window);
	void update(float &heightScale, bool &useDirectLight, bool &useEnvLight, float &lightPos);
	void render();

	~Gui(){};
};
