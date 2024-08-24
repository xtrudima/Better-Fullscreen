//#define DEBUG_CONSOLE // Uncomment this if you want a debug console to start. You can use the Console class to print. You can use Console::inStrings to get input.

#include <4dm.h>

using namespace fdm;

struct Rect
{
	int x, y, w, h;
};

Rect oldWindowRect{0,0,0,0};

// Initialize the DLLMain
initDLL

GLFWmonitor* getCurrentMonitor(GLFWwindow* window, bool fullscreenMode)
{
	if (!fullscreenMode)
	{
		return glfwGetWindowMonitor(window);
	}
	int count;
	GLFWmonitor** monitors = glfwGetMonitors(&count);

	if (!count) return glfwGetPrimaryMonitor();

	Rect windowArea{0,0,0,0};
	glfwGetWindowPos(window, &windowArea.x, &windowArea.y);
	glfwGetWindowSize(window, &windowArea.w, &windowArea.h);

	for (int i = 0; i < count; i++)
	{
		GLFWmonitor* monitor = monitors[i];
		Rect monitorArea{ 0,0,0,0 };
		glfwGetMonitorWorkarea(monitor, &monitorArea.x, &monitorArea.y, &monitorArea.w, &monitorArea.h);
		if (windowArea.x < monitorArea.x + monitorArea.w &&
			windowArea.x + windowArea.w > monitorArea.x &&
			windowArea.y < monitorArea.y + monitorArea.h &&
			windowArea.y + windowArea.h > monitorArea.y)
		{
			return monitor;
		}
	}
	return glfwGetPrimaryMonitor();
}

//		void setFullscreenMode(GLFWwindow* window, bool fullscreenMode) 
$hook(void, StateSettings, setFullscreenMode, GLFWwindow* window, bool fullscreenMode)
{
	if (fullscreenMode)
	{
		glfwGetWindowPos(window, &oldWindowRect.x, &oldWindowRect.y);
		glfwGetWindowSize(window, &oldWindowRect.w, &oldWindowRect.h);
		GLFWmonitor* monitor = getCurrentMonitor(window, fullscreenMode);
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
	}
	else
	{
		glfwSetWindowMonitor(window, NULL, oldWindowRect.x, oldWindowRect.y, oldWindowRect.w, oldWindowRect.h, GLFW_DONT_CARE);
	}
	self->fullscreen = fullscreenMode;
	self->fullscreenButton.setText(fullscreenMode ? "Exit Fullscreen" : "Enter Fullscreen");
}

$hook(void, StateIntro, init, StateManager& s)
{
	glfwInit();
	original(self, s);

	// initialize opengl stuff
	glewExperimental = true;
	glewInit();
}