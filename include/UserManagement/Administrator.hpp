#ifndef ADMINISTRATOR_HPP
#define ADMINISTRATOR_HPP

#include "User.hpp"
#include <string>
#include <memory>

using std::string;

// Forward declaration
class AirlineManagementSystem;

class Administrator : public User
{
private:
	std::shared_ptr<AirlineManagementSystem> system;

	// User Management
	void manageUsers();
	void createNewUser();
	void viewAllUsers();
	void deleteUser();
	void modifyUserInfo();

	// Constructors
	Administrator(const string& username, const string& password);
	explicit Administrator(const string& username);

public:
	
	// Override pure virtual methods from User
	void displayMenu() override;
	void handleMenuChoice(int choice) override;	
	
	friend class User;
};

#endif // ADMINISTRATOR_HPP