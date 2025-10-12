#ifndef BOOKINGAGENT_HPP
#define BOOKINGAGENT_HPP

#include "User.hpp"
#include <string>

using std::string;

// Forward declarations
class FlightManager;

class BookingAgent : public User
{
private:
	FlightManager* flightManager;
	
	// Constructors
	BookingAgent(const string& username, const string& name, const string& email,
	     const string& phoneNumber, UserRole role);
	
public:
	void userMenu() override;
	virtual string getRoleString() const noexcept;
	
	friend class UsersManager;
};

#endif // BOOKINGAGENT_HPP