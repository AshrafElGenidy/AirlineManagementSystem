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
string User::usersFilePath = "../Databases/Users.json";
int User::nextUserId = 1;
std::unordered_map<string, string> User::usernameIndex;

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
	
	this->userId = generateUserId();
	
	// Create user entry in JSON
	json userData;
	userData["username"] = username;
	userData["passwordHash"] = hashPassword(password);
	userData["role"] = static_cast<int>(role);
	userData["name"] = "";
	userData["email"] = "";
	userData["phoneNumber"] = "";
	
	// Save to file
	usersData[this->userId] = userData;
	saveallUsersData(usersData);

	// Update Static Data
	usernameIndex[username] = this->userId;	
	cachedUserData = userData;
	
	std::cout << "User '" << username << "' registered successfully with ID: " << userId << std::endl;
}

User::User(const string& userId) : userId(userId)
{
	// Load and cache user data
	json userData = getUserData();
	if (userData.empty())
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND, "User " + string(userId) + " does not exist.");
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
	auto validChar =  [](char c) 
	{
		return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
	};

	return std::all_of(username.begin(), username.end(), validChar);
}

bool User::validatePassword(const string& password)
{
	// Check minimum length
	return password.length() >= MIN_PASSWORD_LENGTH;
}

// ==================== Authentication ====================

std::optional<string> User::findUserIdByUsername(const string& username) noexcept
{
	auto it = usernameIndex.find(username);
	
	if (it != usernameIndex.end())
	{
		return it->second;
	}
	
	return std::nullopt;
}

std::unique_ptr<User> User::createUserFromId(const string& userId)
{
	json usersData = loadallUsersData();
	
	// Check if user exists
	if (!usersData.contains(userId))
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND, "User " + userId + " not found in database.");
	}
	
	const json& userData = usersData[userId];
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
		throw UserException(UserErrorCode::INVALID_CREDENTIALS, "Invalid password.");
	}
	
	// Successful login
	std::cout << "Login successful! Welcome, " << user->getName() << std::endl;
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
		std::cout << "User " << username << " logged out successfully." << std::endl;
	}
	catch (...)
	{
		std::cout << "User logged out." << std::endl;
	}
}

// ==================== Getters ====================

string User::getUserId() const noexcept
{
	return userId;
}

string User::getUsername() const noexcept
{
	try
	{
		const json& userData = getUserData();
		return userData.value("username", "");
	}
	catch (...)
	{
		return "";
	}
}

string User::getName() const noexcept
{
	try
	{
		const json& userData = getUserData();
		return userData.value("name", "");
	}
	catch (...)
	{
		return "";
	}
}

UserRole User::getRole() const noexcept
{
	try
	{
		const json& userData = getUserData();
		int roleInt = userData.value("role", 0);
		return static_cast<UserRole>(roleInt);
	}
	catch (...)
	{
		UserException(UserErrorCode::DATABASE_ERROR, "Unknown user role.");
	}
}

string User::getEmail() const noexcept
{
	try
	{
		const json& userData = getUserData();
		return userData.value("email", "");
	}
	catch (...)
	{
		return "";
	}
}

string User::getPhoneNumber() const noexcept
{
	try
	{
		const json& userData = getUserData();
		return userData.value("phoneNumber", "");
	}
	catch (...)
	{
		return "";
	}
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
	// Return cached data if available
	if (cachedUserData)
	{
		return *cachedUserData;
	}
	
	// Load from file and cache
	json usersData = loadallUsersData();
	
	if (usersData.contains(userId))
	{
		cachedUserData = usersData[userId];
		return *cachedUserData;
	}
	
	// Return empty object if user not found
	return json::object();
}

void User::updateUserData(const json& updates)
{
	json usersData = loadallUsersData();
	
	if (usersData.contains(userId))
	{
		// Merge updates into existing user data
		for (const auto& [key, value] : updates.items())
		{
			usersData[userId][key] = value;
		}
		
		saveallUsersData(usersData);
		
		// Invalidate cache so next read gets fresh data
		invalidateCache();
	}
	else
	{
		throw UserException(UserErrorCode::USER_NOT_FOUND, "User " + userId + " not found in database.");
	}
}

// ==================== Helpers ====================

void User::invalidateCache()
{
	cachedUserData = std::nullopt;
}

void User::rebuildUsernameIndex()
{
	usernameIndex.clear();
	json usersData = loadallUsersData();
	
	for (const auto& [userId, userData] : usersData.items())
	{
		string username = userData.value("username", "");
		if (!username.empty())
		{
			usernameIndex[username] = userId;
		}
	}
}

// ==================== User Creation ====================

string User::generateUserId()
{
	// Use the pre-calculated nextUserId from initialization
	std::stringstream ss;
	ss << "USER" << std::setw(3) << std::setfill('0') << nextUserId;
	
	// Increment for next user
	nextUserId++;
	
	return ss.str();
}

// Simple password hashing (in production, use bcrypt or similar)
string User::hashPassword(const string& password)
{
	// Simple hash implementation - XOR with constant and convert to hex
	// NOTE: This is NOT secure - use proper hashing in production!
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

	// Create JSON file if it doesn't exist
	std::ifstream testFile(usersFilePath);
	if (!testFile.is_open())
	{
		json emptyData = json::object();
		saveallUsersData(emptyData);
		nextUserId = 1;  // Start from 1 for new database
		std::cout << "Created new Users database at: " << usersFilePath << std::endl;
	}
	else
	{
		testFile.close();
		
		// Load existing data and find the max ID
		json usersData = loadallUsersData();
		int maxId = 0;
		
		for (const auto& [key, value] : usersData.items())
		{
			// Extract number from "USER###" format
			if (key.substr(0, 4) == "USER")
			{
				try
				{
					int id = std::stoi(key.substr(4));
					if (id > maxId)
					{
						maxId = id;
					}
				}
				catch (...)
				{
					// Skip invalid IDs
				}
			}
		}
		
		nextUserId = maxId + 1;  // Set to next available ID
		
		// Build username index for O(1) lookups
		rebuildUsernameIndex();
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