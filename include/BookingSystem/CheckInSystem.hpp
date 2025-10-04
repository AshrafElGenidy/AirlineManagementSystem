#ifndef CHECK_IN_SYSTEM_HPP
#define CHECK_IN_SYSTEM_HPP

#include <string>
#include "BoardingPass.hpp"

class CheckInSystem {
public:
    BoardingPass* checkInPassenger(const std::string& reservationId);
    BoardingPass* onlineCheckIn(const std::string& reservationId);
    bool verifyBoardingPass(BoardingPass* boardingPass);
};

#endif // CHECK_IN_SYSTEM_HPP