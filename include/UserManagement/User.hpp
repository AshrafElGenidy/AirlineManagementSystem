#ifndef USER_HPP
#define USER_HPP

#include <string>

enum class UserRole {
    ADMINISTRATOR,
    BOOKING_AGENT,
    PASSENGER
};

class User {
protected:
    std::string userId;
    std::string username;
    std::string passwordHash;
    UserRole role;
    std::string email;
    std::string phoneNumber;
    std::string name;

public:
    User(const std::string& username, const std::string& password, UserRole role);
    virtual ~User() = default;
    
    virtual void displayMenu() = 0;
    virtual void handleMenuChoice(int choice) = 0;
    
    bool login(const std::string& username, const std::string& password);
    void logout();
    bool verifyPassword(const std::string& password);
    std::string getUserId() const;
    std::string getUsername() const;
    std::string getName() const;
    UserRole getRole() const;
};

#endif // USER_HPP