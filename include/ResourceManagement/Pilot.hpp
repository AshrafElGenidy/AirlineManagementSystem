#ifndef PILOT_HPP
#define PILOT_HPP

#include "CrewMember.hpp"

class Pilot : public CrewMember {
private:
    std::string rank;

public:
    Pilot(const std::string& crewId, const std::string& name, const std::string& rank);
    
    std::string getRole() override;
    std::string getRank() const;
    void displayInfo() override;
};

#endif // PILOT_HPP