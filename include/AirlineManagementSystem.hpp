#ifndef AIRLINE_MANAGEMENT_SYSTEM_HPP
#define AIRLINE_MANAGEMENT_SYSTEM_HPP

#include <map>
#include <vector>
#include <string>
#include "User.hpp"
#include "Reservation.hpp"
#include "ReportGenerator.hpp"
#include "CheckInSystem.hpp"
#include "Aircraft.hpp"
#include "CrewMember.hpp"
#include "Pilot.hpp"
#include "FlightAttendant.hpp"
#include "Flight.hpp"

class AirlineManagementSystem {
private:
    static AirlineManagementSystem* instance;
    std::map<std::string, User*> users;
    std::map<std::string, Flight*> flights;
    std::map<std::string, Aircraft*> aircraft;
    std::map<std::string, Reservation*> reservations;
    std::map<std::string, CrewMember*> crewMembers;
    std::vector<Pilot*> pilots;
    std::vector<FlightAttendant*> flightAttendants;
    User* currentUser;
    ReportGenerator* reportGenerator;
    CheckInSystem* checkInSystem;
    
    AirlineManagementSystem();

public:
    static AirlineManagementSystem* getInstance();
    
    void initialize();
    void start();
    void displayWelcome();
    int selectRole();
    User* login(UserRole role);
    void logout();
    
    bool addFlight(Flight* flight);
    bool removeFlight(const std::string& flightNumber);
    bool updateFlight(const std::string& flightNumber);
    Flight* getFlight(const std::string& flightNumber);
    std::vector<Flight*> getAllFlights();
    std::vector<Flight*> searchFlights(const SearchCriteria& criteria);
    
    bool addUser(User* user);
    bool removeUser(const std::string& userId);
    User* getUser(const std::string& userId);
    User* authenticateUser(const std::string& username, const std::string& password, UserRole role);
    
    bool createReservation(Reservation* reservation);
    Reservation* getReservation(const std::string& reservationId);
    bool cancelReservation(const std::string& reservationId);
    std::vector<Reservation*> getPassengerReservations(const std::string& passengerId);
    
    bool addAircraft(Aircraft* aircraft);
    Aircraft* getAircraft(const std::string& aircraftId);
    std::vector<Aircraft*> getAllAircraft();
    
    bool addCrewMember(CrewMember* crew);
    std::vector<Pilot*> getPilots();
    std::vector<FlightAttendant*> getFlightAttendants();
    bool assignCrewToFlight(const std::string& flightNumber, const std::string& pilotId, 
                           const std::string& attendantId);
    
    std::string generateReservationId();
    bool isSeatAvailable(const std::string& flightNumber, const std::string& seatNumber);
    void shutdown();
};

#endif // AIRLINE_MANAGEMENT_SYSTEM_HPP