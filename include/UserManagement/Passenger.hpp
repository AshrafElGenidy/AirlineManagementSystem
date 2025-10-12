#ifndef PASSENGER_HPP
#define PASSENGER_HPP

#include "User.hpp"
#include <string>

using std::string;

// Forward declarations
class FlightManager;

class Passenger : public User
{
private:
	FlightManager* flightManager;
	
	// Constructors
	Passenger(const string& username, const string& name, const string& email,
	     const string& phoneNumber, UserRole role);
	
public:
	void userMenu() override;
	virtual string getRoleString() const noexcept;
	
	friend class UsersManager;
};

#endif // PASSENGER_HPP