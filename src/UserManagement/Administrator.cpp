#include "Administrator.hpp"
#include "Flight.hpp"
#include "AirlineManagementSystem.hpp"
#include "BookingAgent.hpp"
#include "Passenger.hpp"
#include <iostream>
#include <vector>

// ==================== Constructors ====================

Administrator::Administrator(const string& username, const string& password): User(username, password, UserRole::ADMINISTRATOR)
{
	system = AirlineManagementSystem::getInstance();
}

Administrator::Administrator(const string& username): User(username)
{
	// Verify this user is actually an administrator
	if (getRole() != UserRole::ADMINISTRATOR)
	{
		throw UserException(UserErrorCode::INVALID_INPUTS);
	}
	
	system = AirlineManagementSystem::getInstance();
}

// ==================== Menu Display ====================

void Administrator::displayMenu()
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
}

void Administrator::handleMenuChoice(int choice)
{
	switch(choice)
	{
		case 1:
			Flight::manageFlights();
			break;
		case 2:
			system->manageAircraft();
			break;
		case 3:
			manageUsers();
			break;
		case 4:
			system->generateReports();
			break;
		case 5:
			logout();
			break;
		default:
			ui->printError("Invalid choice. Please try again.");
			ui->pauseScreen();
			break;
	}
}

// ==================== User Management ====================

void Administrator::manageUsers()
{
	while (true)
	{
		ui->clearScreen();
		
		vector<string> options = {
			"Create New User",
			"View All Users",
			"Modify User Information",
			"Delete User",
			"Back to Main Menu"
		};
		
		ui->displayMenu("--- Manage Users ---", options);
		
		int choice = ui->getChoice("Enter choice: ", 1, 5);
		
		switch(choice)
		{
			case 1:
				createNewUser();
				break;
			case 2:
				viewAllUsers();
				break;
			case 3:
				modifyUserInfo();
				break;
			case 4:
				deleteUser();
				break;
			case 5:
				return; // Back to main menu
			default:
				ui->printError("Invalid choice.");
				ui->pauseScreen();
				break;
		}
	}
}

void Administrator::createNewUser()
{
	ui->clearScreen();
	ui->printHeader("CREATE NEW USER");
	
	try
	{
		vector<string> roleOptions = {
			"Administrator",
			"Booking Agent",
			"Passenger"
		};
		
		ui->displayMenu("Select User Role", roleOptions);
		int roleChoice = ui->getChoice("Enter role: ", 1, 3);
		
		string username = ui->getString("Enter username: ");
		string password = ui->getPassword("Enter password: ");
		
		// Create user based on role
		std::unique_ptr<User> newUser;
		
		if (roleChoice == 1)
		{
			newUser = std::make_unique<Administrator>(username, password);
		}
		else if (roleChoice == 2)
		{
			newUser = std::make_unique<BookingAgent>(username, password);
		}
		else if (roleChoice == 3)
		{
			newUser = std::make_unique<Passenger>(username, password);
		}
		
		ui->printSuccess("User created successfully!");
	}
	catch (const UserException& e)
	{
		ui->printError(string(e.what()));
	}
	catch (const std::exception& e)
	{
		ui->printError("Error: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

void Administrator::viewAllUsers()
{
	ui->clearScreen();
	ui->printHeader("ALL USERS");
	
	try
	{
		json usersData = db->loadAll();
		
		if (usersData.empty())
		{
			ui->printWarning("No users found in the system.");
		}
		else
		{
			vector<string> headers = {
				"Username", "Full Name", "Role", "Email", "Phone"
			};
			
			vector<vector<string>> rows;
			
			for (const auto& [username, userData] : usersData.items())
			{
				string name = userData.value("name", "N/A");
				int roleInt = userData.value("role", 0);
				string role;
				
				switch(static_cast<UserRole>(roleInt))
				{
					case UserRole::ADMINISTRATOR:
						role = "Administrator";
						break;
					case UserRole::BOOKING_AGENT:
						role = "Booking Agent";
						break;
					case UserRole::PASSENGER:
						role = "Passenger";
						break;
					default:
						role = "Unknown";
						break;
				}
				
				string email = userData.value("email", "N/A");
				string phone = userData.value("phoneNumber", "N/A");
				
				rows.push_back({username, name, role, email, phone});
			}
			
			ui->displayTable(headers, rows);
			ui->println("\nTotal Users: " + std::to_string(usersData.size()));
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error retrieving users: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

void Administrator::modifyUserInfo()
{
	ui->clearScreen();
	ui->printHeader("MODIFY USER INFORMATION");
	
	try
	{
		string username = ui->getString("Enter username of user to modify: ");
		
		if (!db->entryExists(username))
		{
			ui->printError("User not found.");
			ui->pauseScreen();
			return;
		}
				
		std::unique_ptr<User> user = User::createUserObject(username);
		
		ui->println("\nCurrent Information:");
		ui->println("Name: " + user->getName());
		ui->println("Email: " + user->getEmail());
		ui->println("Phone: " + user->getPhoneNumber());
		
		vector<string> options = {
			"Name",
			"Email",
			"Phone Number",
			"Cancel"
		};
		
		ui->displayMenu("\nWhat would you like to modify?", options);
		int choice = ui->getChoice("Enter choice: ", 1, 4);
		
		switch(choice)
		{
			case 1:
			{
				string newName = ui->getString("Enter new name: ");
				user->setName(newName);
				ui->printSuccess("Name updated successfully.");
				break;
			}
			case 2:
			{
				string newEmail = ui->getString("Enter new email: ");
				user->setEmail(newEmail);
				ui->printSuccess("Email updated successfully.");
				break;
			}
			case 3:
			{
				string newPhone = ui->getString("Enter new phone number: ");
				user->setPhoneNumber(newPhone);
				ui->printSuccess("Phone number updated successfully.");
				break;
			}
			case 4:
				ui->printWarning("Modification canceled.");
				break;
			default:
				ui->printError("Invalid choice.");
				break;
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

void Administrator::deleteUser()
{
	ui->clearScreen();
	ui->printHeader("DELETE USER");
	
	try
	{
		string username = ui->getString("Enter username of user to delete: ");
		
		if (username == getUsername())	// Prevent deleting self
		{
			ui->printError("You cannot delete your own account while logged in.");
			ui->pauseScreen();
			return;
		}
		
		if (!db->entryExists(username))
		{
			ui->printError("User not found.");
			ui->pauseScreen();
			return;
		}
		
		bool confirm = ui->getYesNo("Are you sure you want to delete user '" + username + "'?");
		if (confirm)
		{
			db->deleteEntry(username);
			
			ui->printSuccess("User '" + username + "' has been deleted successfully.");
		}
		else
		{
			ui->printWarning("User deletion canceled.");
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error: " + string(e.what()));
	}
	
	ui->pauseScreen();
}