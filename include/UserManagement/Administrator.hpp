#ifndef ADMINISTRATOR_HPP
#define ADMINISTRATOR_HPP

#include "User.hpp"
#include <string>
#include <memory>

using std::string;

class Administrator : public User
{
private:
	// Constructors
	Administrator(const string& username, const string& name, const string& email,
	     const string& phoneNumber, UserRole role);

	friend class UsersManager;
	
public:
	void userMenu() override;
	virtual string getRoleString() const noexcept;
};

#endif // ADMINISTRATOR_HPP