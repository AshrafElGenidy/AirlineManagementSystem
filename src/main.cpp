#include "AirlineManagementSystem.hpp"
#include <iostream>
#include <exception>

int main()
{
	try
	{
		AirlineManagementSystem* system = AirlineManagementSystem::getInstance();
		
		system->runSystem();		
		system->shutdown();
		
		return 0;
	}
	catch (const std::exception& e)
	{
		std::cerr << "\n[FATAL ERROR] " << e.what() << std::endl;
		std::cerr << "The system has encountered a critical error and must terminate." << std::endl;
		return 1;
	}
	catch (...)
	{
		std::cerr << "\n[FATAL ERROR] An unknown error occurred." << std::endl;
		std::cerr << "The system has encountered a critical error and must terminate." << std::endl;
		return 1;
	}
}