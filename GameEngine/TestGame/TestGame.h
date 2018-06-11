#pragma once
#include <GameEngine\Window.h>
#include <GameEngine\Engine.h>
class TestGame
{
public:
	TestGame();
	~TestGame();

	void Run();

private:
	void InitSystems();
	void GameLoop();
	GameEngine::Window m_window;
	GameEngine::Engine m_engine;
};

