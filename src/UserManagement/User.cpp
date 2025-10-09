#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include "User.hpp"
#include "Administrator.hpp"
#include "BookingAgent.hpp"
#include "Passenger.hpp"

// ==================== User Class ====================

// Static member initialization
UserInterface* User::ui = nullptr;
string User::usersFilePath = "Databases/Users.json";

// ==================== Constructors ====================

User::User(const string& username, const string& password, UserRole role) : username(username)
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
	
	// Load existing users data
	json usersData = loadallUsersData();
	
	// Check if username already exists
	if (usersData.contains(username))
	{
		throw UserException(UserErrorCode::USERNAME_TAKEN);
	}
	
	// Create user entry in JSON
	ui->println("\n--- Enter User Details ---");
	json userData;
	userData["passwordHash"] = hashPassword(password);
	userData["role"] = static_cast<int>(role);
	userData["name"] =  ui->getString("Enter full name: ");;
	userData["email"] =  ui->getString("Enter email address: ");;
	userData["phoneNumber"] = ui->getString("Enter phone number: ");
	
	// Save to file
	usersData[this->username] = userData;
	saveallUsersData(usersData);
	
	ui->printSuccess("User '" + username + "' registered successfully.");
}

User::User(const string& username) : username(username)
{
	// Load user data to verify user exists
	json userData = getUserData();
	if (userData.empty())
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
}

// ==================== Validation Methods ====================

bool User::validateUsername(const string& username)
{
	// Check length
	if (username.length() < MIN_USERNAME_LENGTH || username.length() > MAX_USERNAME_LENGTH)
	{
		return false;
	}
	
	// Check characters (alphanumeric and underscore only)
	auto isValidChar =  [](char c) 
	{
		return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
	};

	return std::all_of(username.begin(), username.end(), isValidChar);
}

bool User::validatePassword(const string& password)
{
	return (password.length() > MIN_PASSWORD_LENGTH && password.length() < MAX_PASSWORD_LENGTH);
}

// ==================== Authentication ====================

std::unique_ptr<User> User::createUserObject(const string& username)
{
	json usersData = loadallUsersData();
	
	// Check if user exists
	if (!usersData.contains(username))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	const json& userData = usersData[username];
	UserRole role = static_cast<UserRole>(userData["role"].get<int>());
	
	// Create appropriate subclass based on role using unique_ptr
	switch(role)
	{
		case UserRole::ADMINISTRATOR:
			return std::make_unique<Administrator>(username);
		case UserRole::BOOKING_AGENT:
			return std::make_unique<BookingAgent>(username);
		case UserRole::PASSENGER:
			return std::make_unique<Passenger>(username);
		default:
			throw UserException(UserErrorCode::DATABASE_ERROR);
	}
}

// validates credentials and returns User object
std::unique_ptr<User> User::login(const string& username, const string& password)
{
	// Load users data to check if username exists
	json usersData = loadallUsersData();
	
	if (!usersData.contains(username))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	// Create user object (ownership transferred to unique_ptr)
	std::unique_ptr<User> user = createUserObject(username);
	
	// Verify password
	if (!user->verifyPassword(password))
	{
		throw UserException(UserErrorCode::INCORRECT_PASSWORD);
	}
	
	// Successful login
	ui->printSuccess("Login successful! Welcome, " + user->getName());
	return user;
}

// Verify password matches stored hash
bool User::verifyPassword(const string& password)
{
	const json& userData = getUserData();
	string storedHash = userData["passwordHash"];
	return hashPassword(password) == storedHash;
}

// Logout current user
void User::logout() noexcept
{
	try
	{
		ui->printSuccess("User " + username + " logged out successfully.");
	}
	catch (...)
	{
		ui->println("User logged out.");
	}
}

// ==================== Getters ====================

string User::getUsername() const noexcept
{
	return username;
}

string User::getName() const
{
	const json& userData = getUserData();
	return userData["name"];
}

UserRole User::getRole() const
{
	const json& userData = getUserData();
	int roleInt = userData["role"];
	return static_cast<UserRole>(roleInt);
}

string User::getEmail() const
{
	const json& userData = getUserData();
	return userData["email"];
}

string User::getPhoneNumber() const
{
	const json& userData = getUserData();
	return userData["phoneNumber"];
}

// ==================== Setters ====================

void User::setName(const string& name)
{
	json updates;
	updates["name"] = name;
	updateUserData(updates);
}


void User::setEmail(const string& email)
{
	json updates;
	updates["email"] = email;
	updateUserData(updates);
}

void User::setPhoneNumber(const string& phoneNumber)
{
	json updates;
	updates["phoneNumber"] = phoneNumber;
	updateUserData(updates);
}

// ==================== JSON Operations ====================

json User::loadallUsersData()
{
	std::ifstream file(usersFilePath);
	
	if (!file.is_open())
	{
		// If file doesn't exist, return empty JSON object
		return json::object();
	}
	
	json data;
	try
	{
		file >> data;
	}
	catch (const json::exception& e)
	{
		throw UserException(UserErrorCode::DATABASE_ERROR);
	}
	
	file.close();
	return data;
}

void User::saveallUsersData(const json& data)
{
	std::ofstream file(usersFilePath);
	
	if (!file.is_open())
	{
		throw UserException(UserErrorCode::DATABASE_ERROR);
	}
	
	try
	{
		file << data.dump(4);
	}
	catch (const json::exception& e)
	{
		throw UserException(UserErrorCode::DATABASE_ERROR);
	}
	
	file.close();
}

json User::getUserData() const
{
	json usersData = loadallUsersData();
	
	if (!usersData.contains(username))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	return usersData[username];
}

void User::updateUserData(const json& updates)
{
	json usersData = loadallUsersData();
	
	if (usersData.contains(username))
	{
		// Merge updates into existing user data
		for (const auto& [key, value] : updates.items())
		{
			usersData[username][key] = value;
		}
		
		saveallUsersData(usersData);
	}
	else
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
}

// ==================== Helpers ====================

string User::hashPassword(const string& password)
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

// ==================== Static initialization ====================

void User::initializeUserSystem()
{
	ui = UserInterface::getInstance();

	// Create JSON file if it doesn't exist
	std::ifstream testFile(usersFilePath);
	if (!testFile.is_open())
	{
		json emptyData = json::object();
		saveallUsersData(emptyData);
	}
	else
	{
		testFile.close();
	}
	
	// Check if this is first-time setup (no users in database)
	json usersData = loadallUsersData();
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
				
				Administrator admin(username, password);
				
				ui->println("\nYou can now login with these credentials.");
				ui->pauseScreen();
				
				adminCreated = true;
			}
			catch (const std::exception& e)
			{
				ui->printError(string(e.what()));
				ui->println("Please try again.");
			}
		}
	}
}

// ==================== UserException Class ====================

UserException::UserException(UserErrorCode code):errorCode(code){}

const char* UserException::what() const noexcept
{
	return getErrorMessage().c_str();
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
			return "Invalid username. Must be " + std::to_string(MIN_USERNAME_LENGTH) + "-" + std::to_string(MAX_USERNAME_LENGTH) + " characters, alphanumeric and underscore only.";
		case UserErrorCode::INVALID_PASSWORD:
			return "Invalid username. Must be " + std::to_string(MIN_PASSWORD_LENGTH) + "-" + std::to_string(MAX_PASSWORD_LENGTH) + " characters, alphanumeric and underscore only.";
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