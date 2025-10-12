#include "Administrator.hpp"
#include "UsersManager.hpp"
#include "FlightManager.hpp"
#include "AircraftManager.hpp"
#include "UserInterface.hpp"
#include <vector>

// ==================== Constructors ====================

Administrator::Administrator(const string& username, const string& name, const string& email,
	const string& phoneNumber, UserRole role)
		: User(username, name, email, phoneNumber, role) 
{
	// Verify this user is actually an administrator
	if (getRole() != UserRole::ADMINISTRATOR)
	{
		throw UserException(UserErrorCode::INVALID_INPUTS);
	}
}

// ==================== Main Menu ====================

void Administrator::userMenu()
{
	UserInterface* ui = UserInterface::getInstance();
	
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Manage Flights",
			"Manage Aircraft",
			"Manage Users",
			"Generate Reports",
			"Logout"
		};
		
		ui->displayMenu("--- Administrator Menu ---", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 5);
			
			switch (choice)
			{
				case 1:
					FlightManager::getInstance()->manageFlights();
					break;
				case 2:
					AircraftManager::getInstance()->manageAircraft();
					break;
				case 3:
					UsersManager::getInstance()->manageUsers();
					break;
				case 4:
					ui->printWarning("Report generation not yet implemented.");
					ui->pauseScreen();
					break;
				case 5:
					ui->printSuccess("User " + username + " logged out successfully.");
					return; // Exit menu loop
				default:
					ui->printError("Invalid choice. Please try again.");
					ui->pauseScreen();
					break;
			}
		}
		catch (const UIException& e)
		{
			ui->printError(string(e.what()));
			ui->pauseScreen();
		}
	}
}

string Administrator::getRoleString() const noexcept
{
	return "Administrator";
}