
#include "window.h"
#include "gui.h"

bool useTexture = false;

int main()
{
	Window mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.initialise();

	Gui gui = Gui();
	gui.initialise(mainWindow.getWindow());

	// Loop until window closed
	while (!mainWindow.getShouldClose())
	{
		// Get + Handle User Input
		glfwPollEvents();

		gui.update(useTexture);

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gui.render();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}