#ifndef BOOKING_AGENT_HPP
#define BOOKING_AGENT_HPP

#include "User.hpp"

class BookingAgent : public User {
private:
    std::string agentId;
    double commissionRate;
    int totalBookings;

public:
    BookingAgent(const std::string& username, const std::string& password);
    
    void displayMenu() override;
    void handleMenuChoice(int choice) override;
    
    void searchFlightsMenu();
    void bookFlightMenu();
    void modifyReservationMenu();
    void cancelReservationMenu();
    bool processRefund(const std::string& reservationId, double amount);
};

#endif // BOOKING_AGENT_HPP