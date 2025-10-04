#ifndef SEAT_MAP_HPP
#define SEAT_MAP_HPP

#include <map>
#include <string>
#include "Seat.hpp"

class SeatMap {
private:
    int totalSeats;
    std::map<std::string, Seat*> seats;

public:
    SeatMap(int totalSeats);
    
    bool isSeatAvailable(const std::string& seatNumber) const;
    bool isValidSeat(const std::string& seatNumber) const;
    bool reserveSeat(const std::string& seatNumber);
    bool releaseSeat(const std::string& seatNumber);
    int getAvailableSeatsCount() const;
};

#endif // SEAT_MAP_HPP