#include "Window.h"

namespace GameEngine
{

	Window::Window()
	{
	}


	Window::~Window()
	{
	}
	int Window::Create()
	{
		//starts up glfw
		glfwInit();

		//gives info on window

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(window_width, window_height, "GameEngine", nullptr, nullptr);

		return 0;
	}
}