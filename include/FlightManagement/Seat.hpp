#ifndef SEAT_HPP
#define SEAT_HPP

#include <string>

enum class SeatClass {
    ECONOMY,
    BUSINESS,
    FIRST_CLASS
};

class Seat {
private:
    std::string seatNumber;
    SeatClass seatClass;
    bool isOccupied;

public:
    Seat(const std::string& seatNumber, SeatClass seatClass);
    
    bool isAvailable() const;
    void reserve();
    void release();
};

#endif // SEAT_HPP