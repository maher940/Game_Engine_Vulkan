#include<stdexcept>
#include<iostream>
#include "TestGame.h"
int main(int argc, char** argv)
{
	TestGame testgame;

	try
	{
		testgame.Run();
	}
	catch(const std::runtime_error& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}