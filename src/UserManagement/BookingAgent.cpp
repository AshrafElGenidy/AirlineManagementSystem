#include "BookingAgent.hpp"
#include "FlightManager.hpp"
#include "UserInterface.hpp"
#include <vector>

// ==================== Constructors ====================

BookingAgent::BookingAgent(const string& username, const string& name, const string& email,
	const string& phoneNumber, UserRole role)
		: User(username, name, email, phoneNumber, role) 
{
	// Verify this user is actually an BookingAgent
	if (getRole() != UserRole::BOOKING_AGENT)
	{
		throw UserException(UserErrorCode::INVALID_INPUTS);
	}
}

// ==================== Menu Display and Handling ====================

void BookingAgent::userMenu()
{
	UserInterface* ui = UserInterface::getInstance();
	
	// Initialize managers on first use
	if (flightManager == nullptr)
		flightManager = FlightManager::getInstance();
	
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Search Flights",
			"Make Reservation",
			"View Reservations",
			"Cancel Reservation",
			"Logout"
		};
		
		ui->displayMenu("--- Booking Agent Menu ---", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 5);
			
			switch (choice)
			{
				case 1:
					ui->printWarning("Search flights functionality not yet implemented.");
					ui->pauseScreen();
					break;
				case 2:
					ui->printWarning("Make reservation functionality not yet implemented.");
					ui->pauseScreen();
					break;
				case 3:
					ui->printWarning("View reservations functionality not yet implemented.");
					ui->pauseScreen();
					break;
				case 4:
					ui->printWarning("Cancel reservation functionality not yet implemented.");
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

string BookingAgent::getRoleString() const noexcept
{
	return "Booking Agent";
}