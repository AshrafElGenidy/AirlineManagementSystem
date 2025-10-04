#ifndef FLIGHT_ATTENDANT_HPP
#define FLIGHT_ATTENDANT_HPP

#include "CrewMember.hpp"
#include <vector>
#include <string>

class FlightAttendant : public CrewMember {
private:
    std::vector<std::string> languages;

public:
    FlightAttendant(const std::string& crewId, const std::string& name);
    
    void displayInfo() override;
	std::string getRole() override;
    std::vector<std::string> getLanguages() const;
};

#endif // FLIGHT_ATTENDANT_HPP