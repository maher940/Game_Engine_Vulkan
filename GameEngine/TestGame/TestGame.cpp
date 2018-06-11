#include "TestGame.h"



TestGame::TestGame()
{
}


TestGame::~TestGame()
{
}

void TestGame::Run()
{
	InitSystems();
}

void TestGame::InitSystems()
{
	m_window.Create();

	m_engine.InitVulkan();

	GameLoop();
}
void TestGame::GameLoop()
{
	while (!glfwWindowShouldClose(m_window.window))
	{
		glfwPollEvents();
	}
}