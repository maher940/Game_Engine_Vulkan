#pragma once
#include <GLFW\glfw3.h>
namespace GameEngine {
	class Window
	{
	public:
		Window();
		~Window();

		int Create();

		GLFWwindow* window;
	private:
		int window_height = 600;
		int window_width = 800;
	};
}
