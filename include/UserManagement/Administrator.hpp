#ifndef ADMINISTRATOR_HPP
#define ADMINISTRATOR_HPP

#include "User.hpp"

class Administrator : public User {
private:
    int adminLevel;

public:
    Administrator(const std::string& username, const std::string& password);
    
    void displayMenu() override;
    void handleMenuChoice(int choice) override;
    
    void manageFlights();
    void addNewFlight();
    void updateExistingFlight();
    void removeFlight();
    void viewAllFlights();
    void manageAircraft();
    void assignCrewMenu(const std::string& flightNumber);
    void manageUsers();
    void generateReports();
    void generateOperationalReport();
};

#endif // ADMINISTRATOR_HPP