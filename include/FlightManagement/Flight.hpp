#ifndef FLIGHT_HPP
#define FLIGHT_HPP

#include <string>
#include "SeatMap.hpp"

enum class FlightStatus {
    SCHEDULED,
    DELAYED,
    BOARDING,
    DEPARTED,
    ARRIVED,
    CANCELED
};

class SearchCriteria {
public:
    std::string origin;
    std::string destination;
    std::string departureDate;
    double maxPrice;
    
    SearchCriteria(const std::string& origin, const std::string& destination, 
                   const std::string& date);
};


class Pilot;
class FlightAttendant;

class Flight {
private:
    std::string flightNumber;
    std::string origin;
    std::string destination;
    std::string departureDateTime;
    std::string arrivalDateTime;
    std::string aircraftType;
    FlightStatus status;
    double price;
    Pilot* assignedPilot;
    FlightAttendant* assignedFlightAttendant;
    SeatMap* seatMap;
    int totalSeats;
    std::string gate;
    std::string boardingTime;

public:
    Flight(const std::string& flightNumber, const std::string& origin, 
           const std::string& destination, const std::string& departureDateTime,
           const std::string& arrivalDateTime, int totalSeats, double price);
    
    std::string getFlightNumber() const;
    std::string getOrigin() const;
    std::string getDestination() const;
    FlightStatus getStatus() const;
    double getPrice() const;
    int getTotalSeats() const;
    int getAvailableSeats() const;
    
    void updateStatus(FlightStatus status);
    bool assignPilot(Pilot* pilot);
    bool assignFlightAttendant(FlightAttendant* attendant);
    void displayFlightInfo();
    bool isSeatAvailable(const std::string& seatNumber) const;
    bool reserveSeat(const std::string& seatNumber);
    bool releaseSeat(const std::string& seatNumber);
};

#endif // FLIGHT_HPP