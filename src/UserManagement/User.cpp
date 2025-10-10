#include <iostream>
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
std::unique_ptr<Database> User::db = nullptr;

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
	
	if (db->entryExists(username))
	{
		throw UserException(UserErrorCode::USERNAME_TAKEN);
	}
	
	// Create user entry in JSON
	ui->println("\n--- Enter User Details ---");
	json userData;
	userData["passwordHash"] = hashPassword(password);
	userData["role"] = static_cast<int>(role);
	userData["name"] = ui->getString("Enter full name: ");
	userData["email"] = ui->getString("Enter email address: ");
	userData["phoneNumber"] = ui->getString("Enter phone number: ");
	
	db->addEntry(username, userData);
	
	ui->printSuccess("User '" + username + "' registered successfully.");
}

User::User(const string& username) : username(username)
{
	if (!db->entryExists(username))
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
	if (!db->entryExists(username))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	json userData = db->getEntry(username);
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
	if (!db->entryExists(username))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND);
	}
	
	std::unique_ptr<User> user = createUserObject(username);
	
	if (!user->verifyPassword(password))
	{
		throw UserException(UserErrorCode::INCORRECT_PASSWORD);
	}
	
	ui->printSuccess("Login successful! Welcome, " + user->getName());
	return user;
}

// Verify password matches stored hash
bool User::verifyPassword(const string& password)
{
	json userData = db->getEntry(username);
	string storedHash = userData["passwordHash"];
	return hashPassword(password) == storedHash;
}

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
	return db->getAttribute(username, "name");
}

UserRole User::getRole() const
{
	int roleInt = db->getAttribute(username, "role");
	return static_cast<UserRole>(roleInt);
}

string User::getEmail() const
{
	return db->getAttribute(username, "email");
}

string User::getPhoneNumber() const
{
	return db->getAttribute(username, "phoneNumber");
}

// ==================== Setters ====================

void User::setName(const string& name)
{
	db->setAttribute(username, "name", name);
}

void User::setEmail(const string& email)
{
	db->setAttribute(username, "email", email);
}

void User::setPhoneNumber(const string& phoneNumber)
{
	db->setAttribute(username, "phoneNumber", phoneNumber);
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
	db = std::make_unique<Database>("Databases/Users.json");

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

UserException::UserException(UserErrorCode code) : errorCode(code) {}

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
			return "Invalid password. Must be " + std::to_string(MIN_PASSWORD_LENGTH) + "-" + std::to_string(MAX_PASSWORD_LENGTH) + " characters.";
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