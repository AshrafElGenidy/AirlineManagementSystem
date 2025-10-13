#include "User.hpp"

// ==================== Constructor ====================

User::User(const string& username, const string& name, const string& email,
           const string& phoneNumber, UserRole role)
	: username(username), name(name), email(email), phoneNumber(phoneNumber),
	  role(role)
{}

// ==================== Getters ====================

string User::getUsername() const noexcept
{
	return username;
}

string User::getName() const noexcept
{
	return name;
}

string User::getEmail() const noexcept
{
	return email;
}

string User::getPhoneNumber() const noexcept
{
	return phoneNumber;
}

UserRole User::getRole() const noexcept
{
	return role;
}

// ==================== Setters ====================

void User::setName(const string& name) noexcept
{
	this->name = name;
}

void User::setEmail(const string& email) noexcept
{
	this->email = email;
}

void User::setPhoneNumber(const string& phoneNumber) noexcept
{
	this->phoneNumber = phoneNumber;
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