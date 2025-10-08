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
int User::nextUserId = 1;
std::unordered_map<string, int> User::usernameIndex;

// ==================== Constructors ====================

User::User(const string& username, const string& password, UserRole role)
{
	// Validate username
	if (!validateUsername(username))
	{
		throw UserException(UserErrorCode::INVALID_USERNAME, 
			"Invalid username. Must be " + std::to_string(MIN_USERNAME_LENGTH) + "-" + 
			std::to_string(MAX_USERNAME_LENGTH) + " characters, alphanumeric and underscore only.");
	}
	
	// Validate password
	if (!validatePassword(password))
	{
		throw UserException(UserErrorCode::INVALID_PASSWORD, 
			"Invalid password. Must be at least " + std::to_string(MIN_PASSWORD_LENGTH) + " characters.");
	}
	
	// Load existing users data
	json usersData = loadallUsersData();
	
	// Check if username already exists using index
	if (usernameIndex.find(username) != usernameIndex.end())
	{
		throw UserException(UserErrorCode::USERNAME_TAKEN, "Username '" + username + "' is already taken.");
	}
	
	this->userId = nextUserId++;
	
	// Create user entry in JSON
	json userData;
	userData["username"] = username;
	userData["passwordHash"] = hashPassword(password);
	userData["role"] = static_cast<int>(role);
	userData["name"] = "";
	userData["email"] = "";
	userData["phoneNumber"] = "";
	
	// Save to file (use integer key directly in JSON)
	usersData[std::to_string(this->userId)] = userData;
	saveallUsersData(usersData);

	// Update username index
	usernameIndex[username] = this->userId;
	
	ui->printSuccess("User '" + username + "' registered successfully with ID: " + formatUserId(userId));
}

User::User(const int& userId) : userId(userId)
{
	// Load user data to verify user exists
	json userData = getUserData();
	if (userData.empty())
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND, "User " + formatUserId(userId) + " does not exist.");
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

std::optional<int> User::findUserIdByUsername(const string& username) noexcept
{
	auto it = usernameIndex.find(username);
	
	if (it != usernameIndex.end())
	{
		return it->second;
	}
	
	return std::nullopt;
}

std::unique_ptr<User> User::createUserFromId(const int& userId)
{
	json usersData = loadallUsersData();
	string userKey = std::to_string(userId);
	
	// Check if user exists
	if (!usersData.contains(userKey))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND, "User " + formatUserId(userId) + " not found in database.");
	}
	
	const json& userData = usersData[userKey];
	UserRole role = static_cast<UserRole>(userData["role"].get<int>());
	
	// Create appropriate subclass based on role using unique_ptr
	switch(role)
	{
		case UserRole::ADMINISTRATOR:
			return std::make_unique<Administrator>(userId);
		case UserRole::BOOKING_AGENT:
			return std::make_unique<BookingAgent>(userId);
		case UserRole::PASSENGER:
			return std::make_unique<Passenger>(userId);
		default:
			throw UserException(UserErrorCode::DATABASE_ERROR, "Unknown user role.");
	}
}

// validates credentials and returns User object
std::unique_ptr<User> User::login(const string& username, const string& password)
{
	// Find user by username using index
	auto userIdOpt = findUserIdByUsername(username);
	
	if (!userIdOpt)
	{
		throw UserException(UserErrorCode::INVALID_CREDENTIALS, "Invalid username.");
	}
	
	// Create user object (ownership transferred to unique_ptr)
	std::unique_ptr<User> user = createUserFromId(*userIdOpt);
	
	// Verify password
	if (!user->verifyPassword(password))
	{
		throw UserException(UserErrorCode::INVALID_CREDENTIALS, "Incorrect password.");
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
		const json& userData = getUserData();
		string username = userData["username"];
		ui->printSuccess("User " + username + " logged out successfully.");
	}
	catch (...)
	{
		ui->println("User logged out.");
	}
}

// ==================== Getters ====================

string User::getUserId() const noexcept
{
	return formatUserId(userId);
}

string User::getUsername() const
{
	const json& userData = getUserData();
	return userData["username"];
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
		throw UserException(UserErrorCode::DATABASE_ERROR, "Failed to parse user database: " + string(e.what()));
	}
	
	file.close();
	return data;
}

void User::saveallUsersData(const json& data)
{
	std::ofstream file(usersFilePath);
	
	if (!file.is_open())
	{
		throw UserException(UserErrorCode::DATABASE_ERROR, "Could not open " + usersFilePath + " for writing.");
	}
	
	try
	{
		file << data.dump(4);
	}
	catch (const json::exception& e)
	{
		throw UserException(UserErrorCode::DATABASE_ERROR, "Failed to write user database: " + string(e.what()));
	}
	
	file.close();
}

json User::getUserData() const
{
	json usersData = loadallUsersData();
	string userKey = std::to_string(userId);
	
	if (!usersData.contains(userKey))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND, "User " + formatUserId(userId) + " not found in database.");
	}
	
	return usersData[userKey];
}

void User::updateUserData(const json& updates)
{
	json usersData = loadallUsersData();
	string userKey = std::to_string(userId);
	
	if (usersData.contains(userKey))
	{
		// Merge updates into existing user data
		for (const auto& [key, value] : updates.items())
		{
			usersData[userKey][key] = value;
		}
		
		saveallUsersData(usersData);
	}
	else
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND, "User " + formatUserId(userId) + " not found in database.");
	}
}

// ==================== Helpers ====================

string User::formatUserId(int id)
{
	std::stringstream ss;
	ss << "USER" << std::setw(3) << std::setfill('0') << id;
	return ss.str();
}

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
		nextUserId = 1;  // Start from 1 for new database
	}
	else
	{
		testFile.close();
		
		// Load existing data and find the max ID
		json usersData = loadallUsersData();
		int maxId = 0;
		
		for (const auto& [key, value] : usersData.items())
		{
			// find max ID
			int id = std::stoi(key);
			if (id > maxId)
			{
				maxId = id;
			}

			// update usernameIndex
			string username = value.value("username", "");
			if (!username.empty())
			{
				usernameIndex[username] = id;
			}
		}
		
		nextUserId = maxId + 1;  // Set to next available ID
	}
	
	// Check if this is first-time setup (no users in database)
	if (usernameIndex.empty())
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

UserException::UserException(UserErrorCode code, const string& message):errorCode(code),errorMessage(message){}

const char* UserException::what() const noexcept
{
	return errorMessage.c_str();
}

UserErrorCode UserException::getErrorCode() const noexcept
{
	return errorCode;
}