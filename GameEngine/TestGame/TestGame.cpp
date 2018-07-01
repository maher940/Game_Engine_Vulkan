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
	

	m_engine.InitVulkan();

	GameLoop();
}
void TestGame::GameLoop()
{
	while (!glfwWindowShouldClose(m_engine.windowobject.window))
	{
		glfwPollEvents();
	}
}