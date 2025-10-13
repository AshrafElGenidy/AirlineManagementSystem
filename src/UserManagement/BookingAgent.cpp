#include "BookingAgent.hpp"
#include "FlightManager.hpp"
#include "ReservationManager.hpp"
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
	
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Search Flights",
			"Create Reservation",
			"View Reservations",
			"Modify Reservation",
			"Cancel Reservation",
			"Logout"
		};
		
		ui->displayMenu("--- Booking Agent Menu ---", options);
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 6);
			
			switch (choice)
			{
				case 1:
					FlightManager::getInstance()->searchFlights();
					break;
				case 2:
					ReservationManager::getInstance()->createReservation(username);
					break;
				case 3:
					ReservationManager::getInstance()->viewReservations(username, UserRole::BOOKING_AGENT);
					break;
				case 4:
					ReservationManager::getInstance()->modifyReservation(username);
					break;
				case 5:
					ReservationManager::getInstance()->cancelReservation(username);
					break;
				case 6:
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

string BookingAgent::getRoleString() const noexcept
{
	return "Booking Agent";
}