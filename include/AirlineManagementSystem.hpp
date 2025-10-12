#ifndef AIRLINEMANAGEMENTSYSTEM_HPP
#define AIRLINEMANAGEMENTSYSTEM_HPP

#include <memory>
#include "UserInterface.hpp"
#include "UsersManager.hpp"
#include "User.hpp"

using std::shared_ptr;

// ==================== AirlineManagementSystem Class ====================

class AirlineManagementSystem
{
private:
	static AirlineManagementSystem* instance;
	
	UserInterface* ui;
	UsersManager* usersManager;
	shared_ptr<User> currentUser;
	
	// Private constructor for singleton
	AirlineManagementSystem();
	
	// helpers
	void displayWelcomeBanner();
	void handleLogin();
	void handleUserSession();
	
	// Delete copy and move constructors/assignments
	AirlineManagementSystem(const AirlineManagementSystem&) = delete;
	AirlineManagementSystem(AirlineManagementSystem&&) = delete;
	AirlineManagementSystem& operator=(const AirlineManagementSystem&) = delete;
	AirlineManagementSystem& operator=(AirlineManagementSystem&&) = delete;

public:
	// Singleton accessor
	static AirlineManagementSystem* getInstance();
	
	// System lifecycle
	void runSystem();
	void shutdown();
	
	// Destructor
	~AirlineManagementSystem() noexcept = default;
};

#endif // AIRLINEMANAGEMENTSYSTEM_HPP