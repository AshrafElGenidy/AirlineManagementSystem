#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <exception>
#include <optional>
#include <memory>
#include <unordered_map>
#include "json.hpp"
#include "UserInterface.hpp"

using std::string;
using nlohmann::json;

enum class UserRole {
	ADMINISTRATOR,
	BOOKING_AGENT,
	PASSENGER
};

enum class UserErrorCode
{
	SUCCESS,
	USERNAME_TAKEN,
	INVALID_USERNAME,
	INVALID_PASSWORD,
	USER_NOT_FOUND,
	INVALID_CREDENTIALS,
	DATABASE_ERROR
};

class User
{
protected:
	int userId;
	
	// Static data members
	static UserInterface* ui;
	static string usersFilePath;
	static int nextUserId;
	static std::unordered_map<string, int> usernameIndex;  // username -> userId map
	
	// Validation constants
	static constexpr int MIN_USERNAME_LENGTH = 3;
	static constexpr int MAX_USERNAME_LENGTH = 20;
	static constexpr int MIN_PASSWORD_LENGTH = 6;
	
	// JSON operations
	static json loadallUsersData();
	static void saveallUsersData(const json& data);
	json getUserData() const;
	void updateUserData(const json& updates);

	// Helpers
	static void rebuildUsernameIndex();
	static string formatUserId(int id);
	static string hashPassword(const string& password);
	
	// Validation methods
	static bool validateUsername(const string& username);
	static bool validatePassword(const string& password);
	bool verifyPassword(const string& password);
	
public:
	// Constructors
	User(const string& username, const string& password, UserRole role);	// For new Users
	explicit User(const int& userId);									// for existing users
	
	virtual ~User() noexcept = default;
	
	// Functions to be handled by child classes
	virtual void displayMenu() = 0;
	virtual void handleMenuChoice(int choice) = 0;
	
	// Authentication methods
	[[nodiscard]] static std::optional<int> findUserIdByUsername(const string& username) noexcept;
	[[nodiscard]] static std::unique_ptr<User> createUserFromId(const int& userId);
	[[nodiscard]] static std::unique_ptr<User> login(const string& username, const string& password);
	void logout() noexcept;
	
	// Getters
	string getUserId() const noexcept;
	string getUsername() const;
	UserRole getRole() const;
	string getEmail() const;
	string getPhoneNumber() const;
	
	// Setters
	void setEmail(const string& email);
	void setPhoneNumber(const string& phoneNumber);
	
	// Static initialization
	static void initializeUserSystem();
};

class UserException : public std::exception
{
private:
	UserErrorCode errorCode;
	string errorMessage;

public:
	UserException(UserErrorCode code, const string& message);
	const char* what() const noexcept override;
	virtual ~UserException() noexcept = default;
	UserErrorCode getErrorCode() const noexcept;
};

#endif // USER_HPP