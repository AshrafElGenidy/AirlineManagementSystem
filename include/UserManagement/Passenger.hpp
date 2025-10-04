#ifndef PASSENGER_HPP
#define PASSENGER_HPP

#include "User.hpp"
#include <vector>
#include <string>


class PassengerPreferences {
public:
    std::string seatPreference;
    std::string mealPreference;
    std::string specialRequests;
};

class Reservation;

class Passenger : public User {
private:
    std::string passengerId;
    std::string dateOfBirth;
    std::string passportNumber;
    std::string nationality;
    std::vector<Reservation*> travelHistory;
    int loyaltyPoints;
    PassengerPreferences preferences;

public:
    Passenger(const std::string& username, const std::string& password, const std::string& name);
    
    void displayMenu() override;
    void handleMenuChoice(int choice) override;
    
    void searchFlightsMenu();
    Reservation* bookFlightFromSearch(const std::string& flightNumber);
    void viewMyReservationsMenu();
    void checkInMenu();
    void earnLoyaltyPoints(int points);
    double redeemLoyaltyPoints(int points);
};

#endif // PASSENGER_HPP