#ifndef RESERVATION_HPP
#define RESERVATION_HPP

#include <string>
#include "Payment.hpp"

enum class ReservationStatus {
    PENDING,
    CONFIRMED,
    CHECKED_IN,
    BOARDED,
    COMPLETED,
    CANCELED
};

class Flight;

class Reservation {
private:
    std::string reservationId;
    std::string passengerId;
    std::string passengerName;
    std::string flightNumber;
    std::string seatNumber;
    ReservationStatus reservationStatus;
    std::string bookingDate;
    Payment* payment;
    double totalCost;

public:
    Reservation(const std::string& passengerId, const std::string& passengerName, 
                Flight* flight, const std::string& seatNumber, double totalCost);
    
    std::string getReservationId() const;
    std::string getPassengerName() const;
    std::string getFlightNumber() const;
    std::string getSeatNumber() const;
    ReservationStatus getStatus() const;
    double getTotalCost() const;
    
    void confirmReservation();
    void cancelReservation();
    void displayBookingSuccess();
};

#endif // RESERVATION_HPP