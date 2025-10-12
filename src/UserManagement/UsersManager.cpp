#include "UsersManager.hpp"
#include "Administrator.hpp"
#include "BookingAgent.hpp"
#include "Passenger.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cctype>

// ==================== Static Member Initialization ====================

UsersManager* UsersManager::instance = nullptr;

// ==================== Singleton ====================

UsersManager* UsersManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new UsersManager();
	}
	return instance;
}

// ==================== Constructor ====================

UsersManager::UsersManager()
{
	ui = UserInterface::getInstance();
	db = std::make_unique<Database>("Users");
}

// ==================== Destructor ====================

UsersManager::~UsersManager() noexcept = default;

// ==================== Validation and Helpers ====================

bool UsersManager::validateUsername(const string& username)
{
	// Check length
	if (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH)
	{
		return false;
	}
	
	// Check characters (alphanumeric and underscore only)
	auto isValidChar = [](char c)
	{
		return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
	};
	
	return std::all_of(username.begin(), username.end(), isValidChar);
}

bool UsersManager::validatePassword(const string& password)
{
	return (password.length() >= MIN_PASSWORD_LENGTH && password.length() <= MAX_PASSWORD_LENGTH);
}

string UsersManager::hashPassword(const string& password)
{
	std::stringstream ss;
	const int salt = 12345;
	
	for (char c : password)
	{
		ss << std::hex << std::setw(2) << std::setfill('0')
		   << (static_cast<int>(c) ^ salt);
	}
	
	return ss.str();
}

// ==================== System Initialization ====================

void UsersManager::initialize()
{
	json usersData = db->loadAll();
	
	if (usersData.empty())
	{
		ui->printHeader("FIRST TIME SETUP");
		ui->println("No users found in the system.");
		ui->println("You must create an Administrator account to continue.");
		ui->printSeparator();
		
		bool adminCreated = false;
		while (!adminCreated)
		{
			try
			{
				ui->println("\n=== Create Administrator Account ===");
				string username = ui->getString("Enter admin username: ");
				string password = ui->getPassword("Enter admin password: ");
				
				createUser(username, password, UserRole::ADMINISTRATOR);
				
				ui->println("\nYou can now login with these credentials.");
				ui->pauseScreen();
				
				adminCreated = true;
			}
			catch (const UserException& e)
			{
				ui->printError(string(e.what()));
				ui->println("Please try again.");
			}
			catch (const UIException& e)
			{
				ui->printError(string(e.what()));
				ui->println("Please try again.");
			}
			catch (const std::exception& e)
			{
				ui->printError("An error occurred: " + string(e.what()));
				ui->println("Please try again.");
			}
		}
	}
}

// ==================== Authentication ====================

shared_ptr<User> UsersManager::login(const string& username, const string& password)
{
	if (!db->entryExists(username))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	json userData = db->getEntry(username);
	string storedHash = userData["passwordHash"];
	
	// Verify password
	string passwordHash = hashPassword(password);
	if (passwordHash != storedHash)
	{
		throw UserException(UserErrorCode::INCORRECT_PASSWORD);
	}
	
	// Load and return appropriate user type
	shared_ptr<User> user = loadUserFromDatabase(username);
	ui->printSuccess("Login successful! Welcome, " + user->getName());
	return user;
}

// ==================== CRUD Operations ====================

void UsersManager::createUser(const string& username, const string& password, UserRole role)
{
	// Validate username
	if (!validateUsername(username))
	{
		throw UserException(UserErrorCode::INVALID_USERNAME);
	}
	
	// Validate password
	if (!validatePassword(password))
	{
		throw UserException(UserErrorCode::INVALID_PASSWORD);
	}
	
	// Check if username already exists
	if (db->entryExists(username))
	{
		throw UserException(UserErrorCode::USERNAME_TAKEN);
	}
	
	// Get user details
	ui->println("\n--- Enter User Details ---");
	string name = ui->getString("Enter full name: ");
	string email = ui->getString("Enter email address: ");
	string phoneNumber = ui->getString("Enter phone number: ");
	
	// Create user type based on role
	shared_ptr<User> user;
	switch (role)
	{
		case UserRole::ADMINISTRATOR:
			user = std::make_shared<User>(new Administrator(username, name, email, phoneNumber, role));
			break;
		case UserRole::BOOKING_AGENT:
			user = std::make_shared<User>(new BookingAgent(username, name, email, phoneNumber, role));
			break;
		case UserRole::PASSENGER:
			user = std::make_shared<User>(new Passenger(username, name, email, phoneNumber, role));
			break;
		default:
			throw UserException(UserErrorCode::INVALID_INPUTS);
	}
	
	// Save to database with password hash
	string passwordHash = hashPassword(password);
	json userData;
	userData["name"] = user->getName();
	userData["email"] = user->getEmail();
	userData["phoneNumber"] = user->getPhoneNumber();
	userData["role"] = static_cast<int>(user->getRole());
	userData["passwordHash"] = passwordHash;
	
	db->addEntry(username, userData);
	
	ui->printSuccess("User '" + username + "' registered successfully.");
}

shared_ptr<User> UsersManager::loadUserFromDatabase(const string& username)
{
	if (!db->entryExists(username))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	json userData = db->getEntry(username);
	
	string name = userData["name"];
	string email = userData["email"];
	string phoneNumber = userData["phoneNumber"];
	UserRole role = static_cast<UserRole>(userData["role"].get<int>());
	
	// Create user type based on role
	shared_ptr<User> user;
	switch (role)
	{
		case UserRole::ADMINISTRATOR:
			user = std::make_shared<User>(new Administrator(username, name, email, phoneNumber, role));
			break;
		case UserRole::BOOKING_AGENT:
			user = std::make_shared<User>(new BookingAgent(username, name, email, phoneNumber, role));
			break;
		case UserRole::PASSENGER:
			user = std::make_shared<User>(new Passenger(username, name, email, phoneNumber, role));
			break;
		default:
			throw UserException(UserErrorCode::DATABASE_ERROR);
	}
	
	return user;
}

void UsersManager::saveUserToDatabase(const shared_ptr<User>& user)
{
	if (!user)
	{
		throw UserException(UserErrorCode::DATABASE_ERROR);
	}
	
	if (!db->entryExists(user->getUsername()))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	json updates;
	updates["name"] = user->getName();
	updates["email"] = user->getEmail();
	updates["phoneNumber"] = user->getPhoneNumber();
	updates["role"] = static_cast<int>(user->getRole());
	
	db->updateEntry(user->getUsername(), updates);
}

void UsersManager::deleteUserFromDatabase(const string& username)
{
	if (!db->entryExists(username))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	db->deleteEntry(username);
}

// ==================== Private Menu Methods ====================

UserRole UsersManager::getUserRoleChoice()
{
	vector<string> roleOptions = {
		"Administrator",
		"Booking Agent",
		"Passenger"
	};
	
	ui->displayMenu("Select User Role", roleOptions);
	int roleChoice = ui->getChoice("Enter role: ", 1, 3);
	
	switch (roleChoice)
	{
		case 1:
			return UserRole::ADMINISTRATOR;
		case 2:
			return UserRole::BOOKING_AGENT;
		case 3:
			return UserRole::PASSENGER;
		default:
			throw UserException(UserErrorCode::INVALID_INPUTS);
	}
}

void UsersManager::displayUsersTable(const vector<shared_ptr<User>>& users, const string& title)
{
	if (title.empty())
	{
		ui->clearScreen();
		ui->printHeader("ALL USERS");
	}
	else
	{
		ui->clearScreen();
		ui->printHeader(title);
	}
	
	if (users.empty())
	{
		ui->printWarning("No users found in the system.");
		return;
	}
	
	vector<string> headers = {
		"Username", "Full Name", "Role", "Email", "Phone"
	};
	
	vector<vector<string>> rows;
	
	for (const auto& user : users)
	{	
		rows.push_back({
			user->getUsername(),
			user->getName(),
			user->getRoleString(),
			user->getEmail(),
			user->getPhoneNumber()
		});
	}
	
	ui->displayTable(headers, rows);
	ui->println("\nTotal Users: " + std::to_string(users.size()));
}

void UsersManager::updateUserDetails(const shared_ptr<User>& user)
{
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
	
	switch (choice)
	{
		case 1:
		{
			string newName = ui->getString("Enter new name: ");
			user->setName(newName);
			saveUserToDatabase(user);
			ui->printSuccess("Name updated successfully.");
			break;
		}
		case 2:
		{
			string newEmail = ui->getString("Enter new email: ");
			user->setEmail(newEmail);
			saveUserToDatabase(user);
			ui->printSuccess("Email updated successfully.");
			break;
		}
		case 3:
		{
			string newPhone = ui->getString("Enter new phone number: ");
			user->setPhoneNumber(newPhone);
			saveUserToDatabase(user);
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

void UsersManager::createNewUser()
{
	ui->clearScreen();
	ui->printHeader("CREATE NEW USER");
	
	try
	{
		UserRole role = getUserRoleChoice();
		string username = ui->getString("Enter username: ");
		string password = ui->getPassword("Enter password: ");
		
		createUser(username, password, role);
		
		ui->printSuccess("User created successfully!");
	}
	catch (const UserException& e)
	{
		ui->printError(string(e.what()));
	}
	catch (const UIException& e)
	{
		ui->printError(string(e.what()));
	}
	catch (const std::exception& e)
	{
		ui->printError("Error: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

void UsersManager::viewAllUsers()
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
			vector<shared_ptr<User>> users;
			for (const auto& [username, userData] : usersData.items())
			{
				try
				{
					users.push_back(loadUserFromDatabase(username));
				}
				catch (const std::exception&)
				{
					continue;
				}
			}
			
			displayUsersTable(users);
		}
	}
	catch (const std::exception& e)
	{
		ui->printError("Error retrieving users: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

void UsersManager::modifyUserInfo()
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
		
		shared_ptr<User> user = loadUserFromDatabase(username);
		updateUserDetails(user);
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

void UsersManager::deleteUser()
{
	ui->clearScreen();
	ui->printHeader("DELETE USER");
	
	try
	{
		string username = ui->getString("Enter username of user to delete: ");
		
		if (!db->entryExists(username))
		{
			ui->printError("User not found.");
			ui->pauseScreen();
			return;
		}
		
		bool confirm = ui->getYesNo("Are you sure you want to delete user '" + username + "'?");
		if (confirm)
		{
			deleteUserFromDatabase(username);
			ui->printSuccess("User '" + username + "' has been deleted successfully.");
		}
		else
		{
			ui->printWarning("User deletion canceled.");
		}
	}
	catch (const UIException& e)
	{
		ui->printError(string(e.what()));
	}
	catch (const std::exception& e)
	{
		ui->printError("Error: " + string(e.what()));
	}
	
	ui->pauseScreen();
}

// ==================== Main Menu ====================

void UsersManager::manageUsers()
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
		
		try
		{
			int choice = ui->getChoice("Enter choice: ", 1, 5);
			
			switch (choice)
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

// ==================== Query Methods ====================

shared_ptr<User> UsersManager::getUser(const string& username)
{
	return loadUserFromDatabase(username);
}

vector<string> UsersManager::getAllUsernames()
{
	json usersData = db->loadAll();
	vector<string> usernames;
	
	for (const auto& [username, userData] : usersData.items())
	{
		usernames.push_back(username);
	}
	
	return usernames;
}

bool UsersManager::userExists(const string& username)
{
	return db->entryExists(username);
}

// ==================== UserException Class ====================

UserException::UserException(UserErrorCode code) : errorCode(code) {}

const char* UserException::what() const noexcept
{
	static string message;
	message = getErrorMessage();
	return message.c_str();
}

UserErrorCode UserException::getErrorCode() const noexcept
{
	return errorCode;
}

string UserException::getErrorMessage() const noexcept
{
	switch (errorCode)
	{
		case UserErrorCode::USERNAME_TAKEN:
			return "Username is already taken. Please choose a different username.";
		case UserErrorCode::INVALID_USERNAME:
			return "Invalid username. Must be " + std::to_string(MIN_USERNAME_LENGTH) + "-" +
				   std::to_string(MAX_USERNAME_LENGTH) + " characters, alphanumeric and underscore only.";
		case UserErrorCode::INVALID_PASSWORD:
			return "Invalid password. Must be between " + std::to_string(MIN_PASSWORD_LENGTH) +
				   " and " + std::to_string(MAX_PASSWORD_LENGTH) + " characters.";
		case UserErrorCode::USER_NOT_FOUND:
			return "User does not exist.";
		case UserErrorCode::INCORRECT_PASSWORD:
			return "Invalid credentials. Please check your username and password.";
		case UserErrorCode::DATABASE_ERROR:
			return "An error occurred while accessing the database.";
		case UserErrorCode::INVALID_INPUTS:
			return "Error in User inputs.";
		default:
			return "An unknown error occurred.";
	}
}