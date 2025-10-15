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

UserException::UserException(const string& message) : message(message) {}

const char* UserException::what() const noexcept
{
	return message.c_str();
}