#ifndef BOARDING_PASS_HPP
#define BOARDING_PASS_HPP

#include <string>
#include "Reservation.hpp"

class BoardingPass {
private:
    std::string boardingPassId;
    std::string reservationId;
    std::string passengerName;
    std::string flightNumber;
    std::string origin;
    std::string destination;
    std::string departureDateTime;
    std::string seatNumber;
    std::string gate;
    std::string boardingTime;

public:
    BoardingPass(Reservation* reservation);
    
    void displayBoardingPass();
    bool verify();
};

#endif // BOARDING_PASS_HPP