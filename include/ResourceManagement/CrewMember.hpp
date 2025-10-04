#ifndef CREW_MEMBER_HPP
#define CREW_MEMBER_HPP

#include <string>

class Flight;

class CrewMember {
protected:
    std::string crewId;
    std::string name;
    std::string licenseNumber;
    double flightHours;
    double maxFlightHours;

public:
    CrewMember(const std::string& crewId, const std::string& name);
    virtual ~CrewMember() = default;
    
    virtual std::string getRole() = 0;
    virtual void displayInfo() = 0;
    
    std::string getCrewId() const;
    std::string getName() const;
    bool canBeAssigned() const;
    bool assignToFlight(Flight* flight);
};

#endif // CREW_MEMBER_HPP