#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <exception>
#include <memory>
#include "json.hpp"

using std::string;
using nlohmann::json;

// ==================== User Class ====================

enum class UserRole 
{
	ADMINISTRATOR,
	BOOKING_AGENT,
	PASSENGER
};

class User
{
protected:
	string username;
	string name;
	string email;
	string phoneNumber;
	UserRole role;
	
	// Private constructor - only UsersManager can create
	User(const string& username, const string& name, const string& email,
	     const string& phoneNumber, UserRole role);
	
	friend class UsersManager;
	
public:
	// Getters
	string getUsername() const noexcept;
	string getName() const noexcept;
	string getEmail() const noexcept;
	string getPhoneNumber() const noexcept;
	UserRole getRole() const noexcept;
	virtual string getRoleString() const noexcept = 0;
	
	// Setters
	void setName(const string& name) noexcept;
	void setEmail(const string& email) noexcept;
	void setPhoneNumber(const string& phoneNumber) noexcept;
	
	virtual ~User() noexcept = default;
	
	// Main menu loop
	virtual void userMenu() = 0;
};

#endif // USER_HPP