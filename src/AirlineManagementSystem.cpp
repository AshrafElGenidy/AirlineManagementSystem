#include "AirlineManagementSystem.hpp"
#include <iostream>

// ==================== Static Member Initialization ====================

AirlineManagementSystem* AirlineManagementSystem::instance = nullptr;

// ==================== Singleton ====================

AirlineManagementSystem* AirlineManagementSystem::getInstance()
{
	if (instance == nullptr)
	{
		instance = new AirlineManagementSystem();
	}
	return instance;
}

// ==================== Constructor ====================

AirlineManagementSystem::AirlineManagementSystem()
{
	ui = UserInterface::getInstance();
	currentUser = nullptr;

	ui->clearScreen();
	displayWelcomeBanner();
	
	ui->println("Initializing Airline Management System...");
	
	try
	{
		usersManager = UsersManager::getInstance();
		
		ui->printSuccess("System initialized successfully!");
	}
	catch (const std::exception& e)
	{
		ui->printError("Initialization failed: " + string(e.what()));
		ui->println("The system may not function correctly.");
	}
}

// ==================== System Lifecycle ====================

void AirlineManagementSystem::runSystem()
{
	bool running = true;
	
	while (running)
	{
		ui->clearScreen();
		displayWelcomeBanner();
		
		vector<string> options = {
			"Login",
			"Register",
			"Exit System"
		};
		
		ui->displayMenu("Main Menu", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 3);
			
			switch (choice)
			{
				case 1:
					handleLogin();
					break;
				case 2:
					usersManager->createNewPassenger();
					break;
				case 3:
					ui->println("\nThank you for using the Airline Management System.");
					ui->println("Goodbye!");
					running = false;
					break;
				default:
					ui->printError("Invalid choice.");
					ui->pauseScreen();
					break;
			}
		}
		catch (const UIException& e)
		{
			ui->printError(string(e.what()));
			ui->pauseScreen();
		}
		catch (const std::exception& e)
		{
			ui->printError("An unexpected error occurred: " + string(e.what()));
			ui->pauseScreen();
		}
	}
}

void AirlineManagementSystem::shutdown()
{
	ui->clearScreen();
	ui->printHeader("SYSTEM SHUTDOWN");
	ui->println("Shutting down Airline Management System...");
	
	// Clean up current user session
	if (currentUser)
	{
		ui->println("Logging out current user...");
		currentUser.reset();
	}
	
	ui->printSuccess("System shutdown complete.");
	ui->println("");
}

// ==================== Private Methods ====================

void AirlineManagementSystem::displayWelcomeBanner()
{
	ui->println("");
	ui->println("====================================================");
	ui->println("||                                                ||");
	ui->println("||           AIRLINE MANAGEMENT SYSTEM            ||");
	ui->println("||                                                ||");
	ui->println("||     Flight Management & Reservation System     ||");
	ui->println("||                                                ||");
	ui->println("====================================================");
	ui->println("");
}

void AirlineManagementSystem::handleLogin()
{
	ui->clearScreen();
	ui->printHeader("USER LOGIN");
	
	try
	{
		string username = ui->getString("Username: ");
		string password = ui->getPassword("Password: ");
		
		// Attempt login
		currentUser = usersManager->login(username, password);
		
		// Login successful - enter user session
		handleUserSession();
		
		// After user session ends (logout)
		currentUser.reset();
	}
	catch (const UserException& e)
	{
		ui->printError(string(e.what()));
		ui->pauseScreen();
	}
	catch (const UIException& e)
	{
		ui->printError(string(e.what()));
		ui->pauseScreen();
	}
	catch (const std::exception& e)
	{
		ui->printError("Login error: " + string(e.what()));
		ui->pauseScreen();
	}
}

void AirlineManagementSystem::handleUserSession()
{
	if (!currentUser)
	{
		ui->printError("No user logged in.");
		return;
	}
	
	// Display welcome message
	ui->clearScreen();
	ui->printHeader("WELCOME");
	ui->println("User: " + currentUser->getName());
	ui->println("Role: " + currentUser->getRoleString());
	ui->println("Username: " + currentUser->getUsername());
	ui->pauseScreen();
	
	// Delegate to user's polymorphic menu
	try
	{
		currentUser->userMenu();
	}
	catch (const std::exception& e)
	{
		ui->printError("Session error: " + string(e.what()));
		ui->pauseScreen();
	}
	
	// User has logged out
	ui->clearScreen();
	ui->printSuccess("You have been logged out successfully.");
	ui->pauseScreen();
}