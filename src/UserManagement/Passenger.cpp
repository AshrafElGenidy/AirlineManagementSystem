#include "Passenger.hpp"
#include "FlightManager.hpp"
#include "ReservationManager.hpp"
#include "UserInterface.hpp"
#include <vector>

// ==================== Constructors ====================

Passenger::Passenger(const string& username, const string& name, const string& email,
	const string& phoneNumber, UserRole role)
		: User(username, name, email, phoneNumber, role) 
{
	// Verify this user is actually an Passenger
	if (getRole() != UserRole::PASSENGER)
	{
		throw UserException("Error in User inputs.");
	}
}

// ==================== Menu Display and Handling ====================

void Passenger::userMenu()
{
	UserInterface* ui = UserInterface::getInstance();
	
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Search Flights",
			"View My Reservations",
			"Check In",
			"Logout"
		};
		
		ui->displayMenu("--- Passenger Menu ---", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 4);
			
			switch (choice)
			{
				case 1:
					FlightManager::getInstance()->searchFlights();
					break;
				case 2:
					ReservationManager::getInstance()->viewReservations(username, UserRole::PASSENGER);
					break;
				case 3:
					ReservationManager::getInstance()->checkIn(username);
					break;
				case 4:
					ui->printSuccess("User " + username + " logged out successfully.");
					return;
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
	}
}

string Passenger::getRoleString() const noexcept
{
	return "Passenger";
}