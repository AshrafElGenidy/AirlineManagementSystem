#ifndef USERSMANAGER_HPP
#define USERSMANAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "User.hpp"
#include "Database.hpp"
#include "UserInterface.hpp"

using std::string;
using std::vector;
using std::unique_ptr;
using std::shared_ptr;

static constexpr int MIN_USERNAME_LENGTH = 3;
static constexpr int MAX_USERNAME_LENGTH = 20;
static constexpr int MIN_PASSWORD_LENGTH = 8;
static constexpr int MAX_PASSWORD_LENGTH = 30;

// ==================== UsersManager ====================

class UsersManager
{
private:
	static UsersManager* instance;
	
	unique_ptr<Database> db;
	UserInterface* ui;
	
	// Private constructor for singleton
	UsersManager();
	
	// Private menu methods
	void createNewUser();
	void viewAllUsers();
	void modifyUserInfo();
	void deleteUser();
	
	// Helper methods
	shared_ptr<User> loadUserFromDatabase(const string& username);
	void saveUserToDatabase(const shared_ptr<User>& user);
	void deleteUserFromDatabase(const string& username);
	
	// Helper to display users in table format
	void displayUsersTable(const vector<shared_ptr<User>>& users, const string& title = "");
	
	// Helper to get user role choice
	UserRole getUserRoleChoice();
	
	// Helper for updating user details
	void updateUserDetails(const shared_ptr<User>& user);
	
	// CRUD methods
	void createUser(const string& username, const string& password, UserRole role);
	shared_ptr<User> loadUserFromDatabase(const string& username);
	void saveUserToDatabase(const shared_ptr<User>& user);
	void deleteUserFromDatabase(const string& username);
	
	// System initialization - creates first admin if needed
	void initialize();
	
	// Validation and utility methods
	static bool validateUsername(const string& username);
	static bool validatePassword(const string& password);
	static string hashPassword(const string& password);
	
public:
	// Singleton accessor
	static UsersManager* getInstance();

	// Delete copy and move constructors/assignments
	UsersManager(const UsersManager&) = delete;
	UsersManager(UsersManager&&) = delete;
	UsersManager& operator=(const UsersManager&) = delete;
	UsersManager& operator=(UsersManager&&) = delete;
	
	// Authentication
	shared_ptr<User> login(const string& username, const string& password);
	
	// Main menu
	void manageUsers();
	
	// Query methods (for other classes to use)
	shared_ptr<User> getUser(const string& username);
	vector<string> getAllUsernames();
	vector<shared_ptr<User>> getAllUsers();
	bool userExists(const string& username);
	
	// Destructor
	~UsersManager() noexcept;
	
};

#endif // USERSMANAGER_HPP